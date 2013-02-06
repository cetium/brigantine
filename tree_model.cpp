// Andrew Naplavkov

#include <algorithm>
#include <brig/database/mysql/command_allocator.hpp>
#include <brig/database/odbc/command_allocator.hpp>
#include <brig/database/oracle/command_allocator.hpp>
#include <brig/database/postgres/command_allocator.hpp>
#include <brig/database/provider.hpp>
#include <brig/database/sqlite/command_allocator.hpp>
#include <brig/gdal/ogr/provider.hpp>
#include <brig/gdal/provider.hpp>
#include <brig/osm/provider.hpp>
#include <exception>
#include <iterator>
#include <QIcon>
#include <QRegExp>
#include "layer.h"
#include "layer_geometry.h"
#include "layer_raster.h"
#include "provider.h"
#include "tree_model.h"
#include "utilities.h"

tree_model::tree_model(QObject* parent)
  : QAbstractItemModel(parent), m_root(0, provider_ptr()), m_order(0)
{}

QModelIndex tree_model::index(int row, int, const QModelIndex& parent) const
{
  if (parent.isValid() && parent.column() != 0) return QModelIndex();
  const tree_item* parent_itm(parent.isValid()? static_cast<tree_item*>(parent.internalPointer()): &m_root);
  if (row >= (int)parent_itm->m_children.size()) return QModelIndex();
  return createIndex(row, 0, parent_itm->m_children[row].get());
}

QModelIndex tree_model::parent(const QModelIndex& idx) const
{
  if (!idx.isValid()) return QModelIndex();
  const tree_item* itm(static_cast<tree_item*>(idx.internalPointer()));
  const tree_item* parent_itm(itm->m_parent);
  if (!parent_itm) return QModelIndex();
  const int row(parent_itm->position());
  if (row < 0) return QModelIndex();
  return createIndex(row, 0, (void*)parent_itm);
}

int tree_model::rowCount(const QModelIndex& parent) const
{
  const tree_item* parent_itm(parent.isValid()? static_cast<tree_item*>(parent.internalPointer()): &m_root);
  return (int)parent_itm->m_children.size();
}

Qt::ItemFlags tree_model::flags(const QModelIndex& idx) const
{
  Qt::ItemFlags res = (Qt::ItemFlags)0;
  if (idx.isValid())
  {
    res = (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    if (is_layer(idx)) res |= Qt::ItemIsUserCheckable;
  }
  return res;
}

QVariant tree_model::data(const QModelIndex& idx, int role) const
{
  if (idx.isValid())
  {
    tree_item* itm(static_cast<tree_item*>(idx.internalPointer()));
    switch (role)
    {
    case Qt::DecorationRole:
      if (is_provider(idx)) return QIcon(itm->get_provider()->get_icon());
      else if (is_layer(idx)) return QIcon(itm->get_layer()->get_icon());
      break;
    case Qt::DisplayRole: return itm->get_string();
    case Qt::CheckStateRole: if (is_layer(idx)) return itm->get_layer().m_checked? Qt::Checked: Qt::Unchecked; break;
    }
  }
  return QVariant();
}

void tree_model::push_back_checked(std::vector<layer_ptr>& lrs) const
{
  using namespace std;
  for (auto i(begin(m_root.m_children)); i != end(m_root.m_children); ++i)
  {
    for (auto j(begin((*i)->m_children)); j != end((*i)->m_children); ++j)
    {
      layer_ptr lr((*j)->get_layer());
      if (lr.m_checked) lrs.push_back(lr);
    }
  }
}

bool tree_model::has_checked() const
{
  std::vector<layer_ptr> lrs;
  push_back_checked(lrs);
  return !lrs.empty();
}

void tree_model::emit_layers()
{
  std::vector<layer_ptr> lrs;
  push_back_checked(lrs);
  emit signal_layers(lrs);
}

bool tree_model::setData(const QModelIndex& idx, const QVariant&, int role)
{
  if (role != Qt::CheckStateRole || !is_layer(idx)) return false;
  static_cast<tree_item*>(idx.internalPointer())->check(++m_order);
  dataChanged(idx, idx);
  emit_layers();
  return true;
}

void tree_model::connect_to(provider_ptr pvd)
{
  std::unique_ptr<tree_item> pvd_itm(new tree_item(&m_root, pvd));

  auto geometries(pvd->get_geometry_layers());
  for (auto iter(std::begin(geometries)); iter != std::end(geometries); ++iter)
    pvd_itm->m_children.emplace_back(new tree_item(pvd_itm.get(), layer_ptr(new layer_geometry(pvd, *iter))));

  auto rasters(pvd->get_raster_layers());
  for (auto iter(std::begin(rasters)); iter != std::end(rasters); ++iter)
    pvd_itm->m_children.emplace_back(new tree_item(pvd_itm.get(), layer_ptr(new layer_raster(pvd, *iter))));

  std::sort
    ( std::begin(pvd_itm->m_children)
    , std::end(pvd_itm->m_children)
    , [](const std::unique_ptr<tree_item>& a, const std::unique_ptr<tree_item>& b){ return a->get_string() < b->get_string(); }
    );

  beginInsertRows(QModelIndex(), int(m_root.m_children.size()), int(m_root.m_children.size()));
  m_root.m_children.push_back(std::move(pvd_itm));
  endInsertRows();
}

void tree_model::connect_gdal(QString file)
{
  connect_to(provider_ptr
    ( new brig::gdal::provider(file.toUtf8().constData())
    , file
    , QString(":/res/gdal.png")
    ));
}

void tree_model::connect_ogr(QString file, QString drv, QString fitted_id)
{
  connect_to(provider_ptr
    ( new brig::gdal::ogr::provider(file.toUtf8().constData(), drv.toUtf8().constData(), fitted_id.toUtf8().constData())
    , file
    , QString(":/res/gdal.png")
    ));
}

void tree_model::connect_mysql(QString host, int port, QString db, QString usr, QString pwd)
{
  connect_to(provider_ptr
    ( new brig::database::provider<true>(std::make_shared<brig::database::mysql::command_allocator>(host.toUtf8().constData(), port, db.toUtf8().constData(), usr.toUtf8().constData(), pwd.toUtf8().constData()))
    , QString("%1:%2/%3").arg(host).arg(port).arg(db)
    , QString(":/res/mysql.png")
    ));
}

void tree_model::connect_odbc(QString dsn)
{
  auto allocator(std::make_shared<brig::database::odbc::command_allocator>(dsn.toUtf8().constData()));

  QString str(dsn);
  str.replace(QRegExp("PWD=\\w*;"), "");

  QString icon;
  switch (std::unique_ptr<brig::database::command>(allocator->allocate())->system())
  {
  default: icon = ":/res/anonymous.png"; break;
  case brig::database::CUBRID: icon = ":/res/cubrid.png"; break;
  case brig::database::DB2: icon = ":/res/db2.png"; break;
  case brig::database::Informix: icon = ":/res/informix.png"; break;
  case brig::database::Ingres: icon = ":/res/ingres.png"; break;
  case brig::database::MS_SQL: icon = ":/res/ms_sql.png"; break;
  case brig::database::MySQL: icon = ":/res/mysql.png"; break;
  case brig::database::Oracle: icon = ":/res/oracle.png"; break;
  case brig::database::Postgres: icon = ":/res/postgres.png"; break;
  case brig::database::SQLite: icon = ":/res/sqlite.png"; break;
  }

  connect_to(provider_ptr(new brig::database::provider<true>(allocator), str, icon));
}

void tree_model::connect_oracle(QString host, int port, QString db, QString usr, QString pwd)
{
  connect_to(provider_ptr
    ( new brig::database::provider<true>(std::make_shared<brig::database::oracle::command_allocator>(host.toUtf8().constData(), port, db.toUtf8().constData(), usr.toUtf8().constData(), pwd.toUtf8().constData()))
    , QString("%1:%2/%3").arg(host).arg(port).arg(db)
    , QString(":/res/oracle.png")
    ));
}

void tree_model::connect_osm(std::shared_ptr<brig::osm::layer> lr)
{
  connect_to(provider_ptr(new brig::osm::provider(lr), QString("OSM/%1").arg(lr->get_name().c_str()), QString(":/res/osm.png")));
}

void tree_model::connect_postgres(QString host, int port, QString db, QString usr, QString pwd)
{
  connect_to(provider_ptr
    ( new brig::database::provider<true>(std::make_shared<brig::database::postgres::command_allocator>(host.toUtf8().constData(), port, db.toUtf8().constData(), usr.toUtf8().constData(), pwd.toUtf8().constData()))
    , QString("%1:%2/%3").arg(host).arg(port).arg(db)
    , QString(":/res/postgres.png")
    ));
}

void tree_model::connect_sqlite(QString file, bool init)
{
  auto allocator(std::make_shared<brig::database::sqlite::command_allocator>(file.toUtf8().constData()));
  if (init)
  {
    std::unique_ptr<brig::database::command> cmd(allocator->allocate());
    cmd->exec("SELECT InitSpatialMetaData();");
  }
  connect_to(provider_ptr
    ( new brig::database::provider<true>(allocator)
    , file
    , QString(":/res/sqlite.png")
    ));
}

bool tree_model::is_provider(const QModelIndex& idx) const
{
  return idx.isValid() && static_cast<tree_item*>(idx.internalPointer())->m_parent == &m_root;
}

provider_ptr tree_model::get_provider(const QModelIndex& idx) const
{
  return idx.isValid()? static_cast<tree_item*>(idx.internalPointer())->get_provider(): provider_ptr();
}

bool tree_model::is_layer(const QModelIndex& idx) const
{
  return idx.isValid() && static_cast<tree_item*>(idx.internalPointer())->m_parent != &m_root;
}

layer_ptr tree_model::get_layer(const QModelIndex& idx) const
{
  return idx.isValid()? static_cast<tree_item*>(idx.internalPointer())->get_layer(): layer_ptr();
}

void tree_model::disconnect(const QModelIndex& idx)
{
  if (!is_provider(idx)) return;
  tree_item* pvd_itm(static_cast<tree_item*>(idx.internalPointer()));
  bool render(std::find_if
    ( std::begin(pvd_itm->m_children)
    , std::end(pvd_itm->m_children)
    , [&](std::unique_ptr<tree_item>& lr_itm){ return lr_itm->get_layer().m_checked; }
    ) != std::end(pvd_itm->m_children));

  emit signal_disconnect(pvd_itm->get_provider());

  beginRemoveRows(QModelIndex(), idx.row(), idx.row());
  auto iter(std::begin(m_root.m_children));
  std::advance(iter, idx.row());
  m_root.m_children.erase(iter);
  endRemoveRows();

  if (render) emit_layers();
}

void tree_model::refresh(const QModelIndex& idx)
{
  if (!is_provider(idx)) return;
  auto pvd_itm(static_cast<tree_item*>(idx.internalPointer()));
  auto pvd(pvd_itm->get_provider());
  std::vector<std::unique_ptr<tree_item>> children;

  try
  {
    auto geometries(pvd->get_geometry_layers());
    for (auto iter(std::begin(geometries)); iter != std::end(geometries); ++iter)
      children.emplace_back(new tree_item(pvd_itm, layer_ptr(new layer_geometry(pvd, *iter))));

    auto rasters(pvd->get_raster_layers());
    for (auto iter(std::begin(rasters)); iter != std::end(rasters); ++iter)
      children.emplace_back(new tree_item(pvd_itm, layer_ptr(new layer_raster(pvd, *iter))));

    std::sort
      ( std::begin(children)
      , std::end(children)
      , [](const std::unique_ptr<tree_item>& a, const std::unique_ptr<tree_item>& b){ return a->get_string() < b->get_string(); }
      );
  }
  catch (const std::exception& e)  { show_message(e.what()); }

  bool render(false);
  for (auto old_iter(std::begin(pvd_itm->m_children)); old_iter != std::end(pvd_itm->m_children); ++old_iter)
  {
    auto old_lr((*old_iter)->get_layer());
    auto old_name((*old_iter)->get_string());
    auto new_iter(std::find_if(std::begin(children), std::end(children), [&](std::unique_ptr<tree_item>& itm){ return old_name  == itm->get_string(); }));
    if (new_iter != std::end(children))
      *new_iter = std::unique_ptr<tree_item>(new tree_item(pvd_itm, old_lr));
    else
    {
      old_lr->reset_table_defs();
      if (old_lr.m_checked) render = true;
    }
  }

  if (!pvd_itm->m_children.empty())
  {
    beginRemoveRows(idx, 0, int(pvd_itm->m_children.size() - 1));
    pvd_itm->m_children.clear();
    endRemoveRows();
  }

  if (!children.empty())
  {
    beginInsertRows(idx, 0, int(children.size() - 1));
    std::swap(pvd_itm->m_children, children);
    endInsertRows();
  }

  if (render) emit_layers();
}

void tree_model::on_refresh(provider_ptr pvd)
{
  using namespace std;
  auto p = find_if(begin(m_root.m_children), end(m_root.m_children), [&](unique_ptr<tree_item>& itm){ return itm->get_provider() == pvd; });
  if (p != end(m_root.m_children)) refresh(index((*p)->position(), 0));
}
