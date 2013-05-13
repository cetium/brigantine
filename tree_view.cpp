// Andrew Naplavkov

#include <algorithm>
#include <brig/database/mysql/command_allocator.hpp>
#include <brig/database/mysql/multithread_init.hpp>
#include <brig/database/odbc/command_allocator.hpp>
#include <brig/database/odbc/multithread_init.hpp>
#include <brig/database/oracle/client_version.hpp>
#include <brig/database/oracle/command_allocator.hpp>
#include <brig/database/oracle/multithread_init.hpp>
#include <brig/database/postgres/command_allocator.hpp>
#include <brig/database/postgres/multithread_init.hpp>
#include <brig/database/provider.hpp>
#include <brig/database/sqlite/command_allocator.hpp>
#include <brig/database/sqlite/multithread_init.hpp>
#include <brig/gdal/multithread_init.hpp>
#include <brig/gdal/ogr/provider.hpp>
#include <brig/gdal/provider.hpp>
#include <brig/osm/layer_aerial.hpp>
#include <brig/osm/layer_cloudmade.hpp>
#include <brig/osm/layer_cycle.hpp>
#include <brig/osm/layer_mapquest.hpp>
#include <brig/osm/layer_standard.hpp>
#include <brig/osm/multithread_init.hpp>
#include <brig/osm/provider.hpp>
#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QInputDialog>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <utility>
#include "dialog_connect.h"
#include "dialog_create.h"
#include "dialog_insert.h"
#include "dialog_odbc.h"
#include "global.h"
#include "layer.h"
#include "provider.h"
#include "task_attributes.h"
#include "task_create.h"
#include "task_drop.h"
#include "task_extent.h"
#include "task_insert.h"
#include "task_proj.h"
#include "task_scale.h"
#include "tree_view.h"
#include "utilities.h"

enum class provider_type {
  SQLite,
  OGR,
  GDAL
};

struct file_open_def {
  provider_type pvd;
  bool writable;
  const char* filter;
  const char* code;
  const char* ext;
};

static const file_open_def s_filters[] = {
{ provider_type::OGR, false, "Arc/Info ASCII Coverage (*.e00)", "AVCE00", "" },
{ provider_type::GDAL, false, "Arc/Info Binary Grid (hdr.adf)", "AIG", "" },
{ provider_type::GDAL, false, "DTED - Military Elevation Data (*.dt0 *.dt1 *.dt2)", "DTED", "" },
{ provider_type::GDAL, false, "ESRI hdr Labelled (*.bil)", "EHdr", "" },
{ provider_type::OGR, true, "ESRI Shapefiles (*.shp)", "ESRI Shapefile", "shp" },
{ provider_type::GDAL, false, "GeoTIFF (*.tif *.tiff)", "GTiff", "" },
{ provider_type::OGR, false, "GPX (*.gpx)", "GPX", "" }, // layers: waypoints, routes, tracks, ...
{ provider_type::OGR, false, "Mapinfo (*.mif *.tab)", "MapInfo File", "" }, // update of existing files is not currently supported (1.10)
{ provider_type::OGR, false, "OpenStreetMap PBF (*.pbf)", "OSM", "" },
{ provider_type::OGR, false, "S-57 Base file (*.000)", "S57", "" },
{ provider_type::SQLite, true, "SQLite (*.sqlite)", "", "sqlite" },
{ provider_type::GDAL, false, "USGS ASCII DEM / CDED (*.dem)", "USGSDEM", "" },
};

tree_view::tree_view(QWidget* parent) : QTreeView(parent), m_mdl(0)
{
  setModel(&m_mdl);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setHeaderHidden(true);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_show_menu(QPoint)));

  m_connect_mysql_act = new QAction(QIcon(":/res/mysql.png"), "connect to MySQL", this);
  m_connect_mysql_act->setIconVisibleInMenu(true);
  connect(m_connect_mysql_act, SIGNAL(triggered()), this, SLOT(on_connect_mysql()));

  m_connect_odbc_act = new QAction(QIcon(":/res/odbc.png"), "connect using ODBC", this);
  m_connect_odbc_act->setIconVisibleInMenu(true);
  connect(m_connect_odbc_act, SIGNAL(triggered()), this, SLOT(on_connect_odbc()));

  m_connect_oracle_act = new QAction(QIcon(":/res/oracle.png"), "connect to Oracle", this);
  m_connect_oracle_act->setIconVisibleInMenu(true);
  connect(m_connect_oracle_act, SIGNAL(triggered()), this, SLOT(on_connect_oracle()));

  m_connect_osm_act = new QAction(QIcon(":/res/osm.png"), "connect to OSM", this);
  m_connect_osm_act->setIconVisibleInMenu(true);
  connect(m_connect_osm_act, SIGNAL(triggered()), this, SLOT(on_connect_osm()));

  m_connect_postgres_act = new QAction(QIcon(":/res/postgres.png"), "connect to Postgres", this);
  m_connect_postgres_act->setIconVisibleInMenu(true);
  connect(m_connect_postgres_act, SIGNAL(triggered()), this, SLOT(on_connect_postgres()));

  m_open_file_act = new QAction(QIcon(":/res/open.png"), "open file", this);
  m_open_file_act->setIconVisibleInMenu(true);
  connect(m_open_file_act, SIGNAL(triggered()), this, SLOT(on_open_file()));

  m_new_file_act = new QAction(QIcon(":/res/new_file.png"), "new file", this);
  m_new_file_act->setIconVisibleInMenu(true);
  connect(m_new_file_act, SIGNAL(triggered()), this, SLOT(on_new_file()));

  m_copy_checked_act = new QAction(QIcon(":/res/copy.png"), "copy checked layer(s)", this);
  m_copy_checked_act->setIconVisibleInMenu(true);
  connect(m_copy_checked_act, SIGNAL(triggered()), this, SLOT(on_copy_checked()));

  m_refresh_act = new QAction(QIcon(":/res/refresh.png"), "refresh", this);
  m_refresh_act->setIconVisibleInMenu(true);
  connect(m_refresh_act, SIGNAL(triggered()), this, SLOT(on_refresh()));

  m_sql_console_act = new QAction(QIcon(":/res/sql.png"), "SQL console", this);
  m_sql_console_act->setIconVisibleInMenu(true);
  connect(m_sql_console_act, SIGNAL(triggered()), this, SLOT(on_sql_console()));

  m_paste_layers_act = new QAction(QIcon(":/res/paste.png"), "paste layer(s)", this);
  m_paste_layers_act->setIconVisibleInMenu(true);
  connect(m_paste_layers_act, SIGNAL(triggered()), this, SLOT(on_paste_layers()));

  m_disconnect_act = new QAction(QIcon(":/res/disconnect.png"), "disconnect", this);
  m_disconnect_act->setIconVisibleInMenu(true);
  connect(m_disconnect_act, SIGNAL(triggered()), this, SLOT(on_disconnect()));

  m_zoom_to_fit_act = new QAction(QIcon(":/res/zoom.png"), "zoom to fit", this);
  m_zoom_to_fit_act->setIconVisibleInMenu(true);
  connect(m_zoom_to_fit_act, SIGNAL(triggered()), this, SLOT(on_zoom_to_fit()));

  m_use_projection_act = new QAction(QIcon(":/res/map.png"), "use this projection", this);
  m_use_projection_act->setIconVisibleInMenu(true);
  connect(m_use_projection_act, SIGNAL(triggered()), this, SLOT(on_use_projection()));

  m_snap_to_pixels_act = new QAction(QIcon(":/res/pixels.png"), "snap to pixels", this);
  m_snap_to_pixels_act->setIconVisibleInMenu(true);
  connect(m_snap_to_pixels_act, SIGNAL(triggered()), this, SLOT(on_snap_to_pixels()));

  m_attributes_act = new QAction(QIcon(":/res/sql.png"), "attributes", this);
  m_attributes_act->setIconVisibleInMenu(true);
  connect(m_attributes_act, SIGNAL(triggered()), this, SLOT(on_attributes()));

  m_copy_act = new QAction(QIcon(":/res/copy.png"), "copy this layer", this);
  m_copy_act->setIconVisibleInMenu(true);
  connect(m_copy_act, SIGNAL(triggered()), this, SLOT(on_copy()));

  m_paste_rows_act = new QAction(QIcon(":/res/paste.png"), "paste rows", this);
  m_paste_rows_act->setIconVisibleInMenu(true);
  connect(m_paste_rows_act, SIGNAL(triggered()), this, SLOT(on_paste_rows()));

  m_drop_act = new QAction(QIcon(":/res/delete.png"), "drop", this);
  m_drop_act->setIconVisibleInMenu(true);
  connect(m_drop_act, SIGNAL(triggered()), this, SLOT(on_drop()));

  m_separator1_act = new QAction("", this);
  m_separator1_act->setSeparator(true);
  m_separator2_act = new QAction("", this);
  m_separator2_act->setSeparator(true);

  qRegisterMetaType<provider_ptr>("provider_ptr");
  qRegisterMetaType<std::vector<layer_ptr>>("std::vector<layer_ptr>");
  connect(&m_mdl, SIGNAL(signal_layers(std::vector<layer_ptr>)), this, SLOT(emit_layers(std::vector<layer_ptr>)));
  connect(&m_mdl, SIGNAL(signal_disconnect(provider_ptr)), this, SLOT(emit_disconnect(provider_ptr)));
}

void tree_view::connect_by(const std::vector<std::shared_ptr<task_connect::provider_allocator>>& allocators)
{
  qRegisterMetaType<provider_ptr>("provider_ptr");
  qRegisterMetaType<std::vector<layer_ptr>>("std::vector<layer_ptr>");
  for (auto itr(std::begin(allocators)); itr != std::end(allocators); ++itr)
  {
    task_connect* tsk(new task_connect(*itr));
    connect(tsk, SIGNAL(signal_connected(provider_ptr, std::vector<layer_ptr>)), &m_mdl, SLOT(on_connected(provider_ptr, std::vector<layer_ptr>)));
    emit signal_task(std::shared_ptr<task>(tsk));
  }
}

void tree_view::on_connect_mysql()
{
  struct provider_allocator : task_connect::provider_allocator {
    QString host;
    int port;
    QString db, usr, pwd;
    provider_allocator(QString host_, int port_, QString db_, QString usr_, QString pwd_)
      : host(host_), port(port_), db(db_), usr(usr_), pwd(pwd_)
      {}
    QString get_string() override
      { return QString("%1:%2/%3").arg(host).arg(port).arg(db); }
    provider_ptr allocate() override
      {
        return provider_ptr
          ( new brig::database::provider<true>(std::make_shared<brig::database::mysql::command_allocator>
            ( host.toUtf8().constData()
            , port, db.toUtf8().constData()
            , usr.toUtf8().constData()
            , pwd.toUtf8().constData()
            ))
          , get_string()
          , QString(":/res/mysql.png")
          );
      }
  }; // provider_allocator

  dialog_connect dlg(this, QIcon(":/res/mysql.png"), SettingsMySQL, "192.168.1.152", 3306, "test", "root");
  if (dlg.exec() != QDialog::Accepted) return;

  brig::database::mysql::multithread_init();
  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd()));
  connect_by(allocators);
}

void tree_view::on_connect_odbc()
{
  struct provider_allocator : task_connect::provider_allocator {
    QString dsn;
    explicit provider_allocator(QString dsn_)
      : dsn(dsn_)
      {}
    QString get_string() override
      {
        QString str(dsn);
        str.replace(QRegExp("PWD=\\w*;"), "");
        return str;
      }
    provider_ptr allocate() override
      {
        auto allocator(std::make_shared<brig::database::odbc::command_allocator>(dsn.toUtf8().constData()));
        QString icon;
        switch (std::unique_ptr<brig::database::command>(allocator->allocate())->system())
        {
        default: icon = ":/res/anonymous.png"; break;
        case brig::database::DBMS::CUBRID: icon = ":/res/cubrid.png"; break;
        case brig::database::DBMS::DB2: icon = ":/res/db2.png"; break;
        case brig::database::DBMS::Informix: icon = ":/res/informix.png"; break;
        case brig::database::DBMS::Ingres: icon = ":/res/ingres.png"; break;
        case brig::database::DBMS::MS_SQL: icon = ":/res/ms_sql.png"; break;
        case brig::database::DBMS::MySQL: icon = ":/res/mysql.png"; break;
        case brig::database::DBMS::Oracle: icon = ":/res/oracle.png"; break;
        case brig::database::DBMS::Postgres: icon = ":/res/postgres.png"; break;
        case brig::database::DBMS::SQLite: icon = ":/res/sqlite.png"; break;
        }
        return provider_ptr(new brig::database::provider<true>(allocator), get_string(), icon);
      }
  }; // provider_allocator

  dialog_odbc dlg(this);
  if (dlg.exec() != QDialog::Accepted) return;

  brig::database::odbc::multithread_init();
  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(dlg.str()));
  connect_by(allocators);
}

void tree_view::on_connect_oracle()
{
  struct provider_allocator : task_connect::provider_allocator {
    QString host;
    int port;
    QString db, usr, pwd;
    provider_allocator(QString host_, int port_, QString db_, QString usr_, QString pwd_)
      : host(host_), port(port_), db(db_), usr(usr_), pwd(pwd_)
      {}
    QString get_string() override
      { return QString("%1:%2/%3").arg(host).arg(port).arg(db); }
    provider_ptr allocate() override
    {
      return provider_ptr
        ( new brig::database::provider<true>(std::make_shared<brig::database::oracle::command_allocator>
          ( host.toUtf8().constData()
          , port
          , db.toUtf8().constData()
          , usr.toUtf8().constData()
          , pwd.toUtf8().constData()
          ))
        , get_string()
        , QString(":/res/oracle.png")
        );
    }
  }; // provider_allocator

  dialog_connect dlg(this, QIcon(":/res/oracle.png"), SettingsOracle, "192.168.1.152", 1521, "XE", "SYSTEM");
  if (dlg.exec() != QDialog::Accepted) return;

  brig::database::oracle::multithread_init();
  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd()));
  connect_by(allocators);
}

void tree_view::on_connect_osm()
{
  struct provider_allocator : task_connect::provider_allocator {
    std::shared_ptr<brig::osm::layer> lr;
    explicit provider_allocator(std::shared_ptr<brig::osm::layer> lr_)
      : lr(lr_)
      {}
    QString get_string() override
      { return QString("OSM/%1").arg(lr->get_name().c_str()); }
    provider_ptr allocate() override
      { return provider_ptr(new brig::osm::provider(lr), get_string(), QString(":/res/osm.png")); }
  }; // provider_allocator

  std::vector<std::shared_ptr<brig::osm::layer>> lrs;
  lrs.push_back(std::make_shared<brig::osm::layer_aerial>());
  lrs.push_back(std::make_shared<brig::osm::layer_cloudmade>());
  lrs.push_back(std::make_shared<brig::osm::layer_cycle>());
  lrs.push_back(std::make_shared<brig::osm::layer_mapquest>());
  lrs.push_back(std::make_shared<brig::osm::layer_standard>());

  QStringList items;
  for (auto lr(begin(lrs)); lr != end(lrs); ++lr)
    items.push_back(QString::fromUtf8((*lr)->get_name().c_str()));

  auto wnd(QApplication::activeWindow());
  auto flags
    ( wnd->windowFlags()
    & ~Qt::WindowMaximizeButtonHint
    & ~Qt::WindowMinimizeButtonHint
    & ~Qt::WindowContextHelpButtonHint
    );
  bool ok(false);
  QString item(QInputDialog::getItem(wnd, "OpenStreetMap", "Layers", items, items.size() - 1, false, &ok, flags));
  if (ok)
    for (auto lr(begin(lrs)); lr != end(lrs); ++lr)
      if ((*lr)->get_name().compare(item.toUtf8().constData()) == 0)
      {
        brig::osm::multithread_init();
        std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
        allocators.push_back(std::make_shared<provider_allocator>(*lr));
        connect_by(allocators);
      }
}

void tree_view::on_connect_postgres()
{
  struct provider_allocator : task_connect::provider_allocator {
    QString host;
    int port;
    QString db, usr, pwd;
    provider_allocator(QString host_, int port_, QString db_, QString usr_, QString pwd_)
      : host(host_), port(port_), db(db_), usr(usr_), pwd(pwd_)
      {}
    QString get_string() override
      { return  QString("%1:%2/%3").arg(host).arg(port).arg(db); }
    provider_ptr allocate() override
      {
        return provider_ptr
          ( new brig::database::provider<true>(std::make_shared<brig::database::postgres::command_allocator>
            ( host.toUtf8().constData()
            , port
            , db.toUtf8().constData()
            , usr.toUtf8().constData()
            , pwd.toUtf8().constData()
            ))
          , get_string()
          , QString(":/res/postgres.png")
          );
      }
  }; // provider_allocator

  dialog_connect dlg(this, QIcon(":/res/postgres.png"), SettingsPostgres, "gis-lab.info", 5432, "osm_shp", "guest");
  if (dlg.exec() != QDialog::Accepted) return;

  brig::database::postgres::multithread_init();
  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd()));
  connect_by(allocators);
}

void tree_view::on_open_file()
{
  struct provider_allocator : task_connect::provider_allocator {
    provider_type pvd;
    QString file;
    provider_allocator(provider_type pvd_, QString file_)
      : pvd(pvd_), file(file_)
      {}
    QString get_string() override
      { return file; }
    provider_ptr allocate() override
      {
        switch (pvd)
        {
        default: break;
        case provider_type::SQLite:
          {
          auto allocator(std::make_shared<brig::database::sqlite::command_allocator>(file.toUtf8().constData()));
          return provider_ptr(new brig::database::provider<true>(allocator), file, QString(":/res/sqlite.png"));
          }
        case provider_type::OGR:
          return provider_ptr(new brig::gdal::ogr::provider(file.toUtf8().constData()), file, QString(":/res/gdal.png"));
        case provider_type::GDAL:
          return provider_ptr(new brig::gdal::provider(file.toUtf8().constData()), file, QString(":/res/gdal.png"));
        }
        return provider_ptr();
      }
  }; // provider_allocator

  QSettings settings(SettingsIni, QSettings::IniFormat);
  QStringList filters;
  for (auto iter(std::begin(s_filters)); iter != std::end(s_filters); ++iter)
    filters << iter->filter;
  QFileDialog dlg
    ( this
    , "open files"
    , settings.value(QString("%1/%2").arg(SettingsFileOpen).arg(SettingsPath), QDir::currentPath()).toString()
    );
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFiles);
  dlg.setNameFilters(filters);
  dlg.selectNameFilter(settings.value(QString("%1/%2").arg(SettingsFileOpen).arg(SettingsFilter), QString(s_filters[0].filter)).toString());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (dlg.exec() != QDialog::Accepted) return;

  const size_t filter_selected(std::distance(std::begin(s_filters), std::find_if
    ( std::begin(s_filters)
    , std::end(s_filters)
    , [&dlg](const file_open_def& i){ return dlg.selectedNameFilter().compare(i.filter) == 0; }
    )));
  QStringList files(dlg.selectedFiles());
  brig::database::sqlite::multithread_init();
  brig::gdal::multithread_init();
  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  for (int i(0); i < files.size(); ++i)
  {
    if (i == 0)
    {
      settings.setValue(QString("%1/%2").arg(SettingsFileOpen).arg(SettingsPath), QFileInfo(files[0]).absolutePath());
      settings.setValue(QString("%1/%2").arg(SettingsFileOpen).arg(SettingsFilter), dlg.selectedNameFilter());
    }
    allocators.push_back(std::make_shared<provider_allocator>(s_filters[filter_selected].pvd, files[i]));
  }
  connect_by(allocators);
}

void tree_view::on_new_file()
{
  struct provider_allocator : task_connect::provider_allocator {
    provider_type pvd;
    QString file, drv, fitted_id;
    provider_allocator(provider_type pvd_, QString file_, QString drv_, QString fitted_id_)
      : pvd(pvd_), file(file_), drv(drv_), fitted_id(fitted_id_)
      {}
    QString get_string() override
      { return file; }
    provider_ptr allocate() override
      {
        switch (pvd)
        {
        default: break;
        case provider_type::SQLite:
          {
          auto allocator(std::make_shared<brig::database::sqlite::command_allocator>(file.toUtf8().constData()));
          std::unique_ptr<brig::database::command> cmd(allocator->allocate());
          cmd->exec("SELECT InitSpatialMetaData();");
          return provider_ptr(new brig::database::provider<true>(allocator), file, QString(":/res/sqlite.png"));
          }
        case provider_type::OGR:
          return provider_ptr(new brig::gdal::ogr::provider(file.toUtf8().constData(), drv.toUtf8().constData(), fitted_id.toUtf8().constData()), file, QString(":/res/gdal.png"));
        }
        return provider_ptr();
      }
  }; // provider_allocator

  QSettings settings(SettingsIni, QSettings::IniFormat);
  QStringList filters;
  for (auto iter(std::begin(s_filters)); iter != std::end(s_filters); ++iter)
    if (iter->writable)
      filters << iter->filter;
  QFileDialog dlg
    ( this
    , "new file"
    , settings.value(QString("%1/%2").arg(SettingsFileNew).arg(SettingsPath), QDir::currentPath()).toString()
    );
  dlg.setAcceptMode(QFileDialog::AcceptSave);
  dlg.setFileMode(QFileDialog::AnyFile);
  dlg.setNameFilters(filters);
  dlg.selectNameFilter(settings.value(QString("%1/%2").arg(SettingsFileNew).arg(SettingsFilter), QString(s_filters[0].filter)).toString());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (!dlg.exec()) return;

  QFileInfo info(dlg.selectedFiles()[0]);
  settings.setValue(QString("%1/%2").arg(SettingsFileNew).arg(SettingsPath), info.absolutePath());
  settings.setValue(QString("%1/%2").arg(SettingsFileNew).arg(SettingsFilter), dlg.selectedNameFilter());
  const size_t filter_selected(std::distance(std::begin(s_filters), std::find_if
    ( std::begin(s_filters)
    , std::end(s_filters)
    , [&dlg](const file_open_def& i){ return dlg.selectedNameFilter().compare(i.filter) == 0; }
    )));
  if (info.suffix().isEmpty()) info = QFileInfo(info.dir(), QString("%1.%2").arg(info.fileName()).arg(s_filters[filter_selected].ext));
  if (info.exists() && !QFile::remove(info.filePath()))
  {
    show_message("file removing error");
    return;
  }

  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(s_filters[filter_selected].pvd, info.absoluteFilePath(), QString(s_filters[filter_selected].code), info.baseName()));
  connect_by(allocators);
}

void tree_view::on_copy_checked()
{
  m_lrs_copy.clear();
  m_mdl.push_back_checked(m_lrs_copy);
}

void tree_view::on_copy()
{
  m_lrs_copy.clear();
  if (m_mdl.is_layer(m_idx_menu)) m_lrs_copy.push_back( m_mdl.get_layer(m_idx_menu) );
}

void tree_view::on_update()
{
  auto lr(m_mdl.get_layer(m_idx_menu));
  m_paste_layers_act->setEnabled
    (  m_mdl.is_provider(m_idx_menu)
    && !m_lrs_copy.empty()
    );
  m_drop_act->setEnabled
    (  bool(lr)
    );
  m_paste_rows_act->setEnabled
    (  bool(lr)
    && m_lrs_copy.size() == 1
    && lr->get_levels() == m_lrs_copy.front()->get_levels()
    );
}

void tree_view::on_show_menu(QPoint point)
{
  m_idx_menu = indexAt(point);
  on_update();

  QList<QAction*> actions;
  if (m_mdl.is_provider(m_idx_menu))
  {
    actions.append(m_refresh_act);
    if (m_mdl.get_provider(m_idx_menu)->is_database()) actions.append(m_sql_console_act);
    if (!m_mdl.get_provider(m_idx_menu)->is_readonly()) actions.append(m_paste_layers_act);
    actions.append(m_disconnect_act);
    actions.append(m_separator1_act);
  }
  else if (m_mdl.is_layer(m_idx_menu))
  {
    actions.append(m_zoom_to_fit_act);
    actions.append(m_use_projection_act);
    if (m_mdl.get_layer(m_idx_menu)->is_raster()) actions.append(m_snap_to_pixels_act);
    actions.append(m_attributes_act);
    actions.append(m_copy_act);
    if (!m_mdl.get_layer(m_idx_menu)->get_provider()->is_readonly())
    {
      actions.append(m_paste_rows_act);
      actions.append(m_drop_act);
    }
    actions.append(m_separator1_act);
  }
  actions.append(m_connect_mysql_act);
  actions.append(m_connect_odbc_act);
  if (!brig::database::oracle::client_version().empty()) actions.append(m_connect_oracle_act);
  actions.append(m_connect_osm_act);
  actions.append(m_connect_postgres_act);
  actions.append(m_open_file_act);
  actions.append(m_new_file_act);
  actions.append(m_separator2_act);
  actions.append(m_copy_checked_act);
  QMenu::exec(actions, mapToGlobal(point));
}

void tree_view::on_attributes()
{
  if (!m_mdl.is_layer(m_idx_menu)) return;
  qRegisterMetaType<std::shared_ptr<rowset_model>>("std::shared_ptr<rowset_model>");
  task_attributes* tsk(new task_attributes(m_mdl.get_layer(m_idx_menu)));
  connect(tsk, SIGNAL(signal_rowset(std::shared_ptr<rowset_model>)), this, SLOT(emit_rowset(std::shared_ptr<rowset_model>)));
  emit signal_task(std::shared_ptr<task>(tsk));
}

void tree_view::on_remove(const QModelIndex& parent, int start, int end, QModelIndex& index)
{
  if (!index.isValid())
    return;
  else if (!parent.isValid())
  {
    const int parent_row(static_cast<tree_item*>(index.internalPointer())->m_parent->position());
    if (parent_row >= start && parent_row <= end) index = QModelIndex();
  }
  else if (index.parent() == parent)
  {
    if (index.row() > end)
      index = index.model()->index(index.row() - (end - start + 1), index.column(), index.parent());
    else if (index.row() >= start)
      index = QModelIndex();
  }
}

void tree_view::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
  on_remove(parent, start, end, m_idx_menu);
  on_update();
  QTreeView::rowsAboutToBeRemoved(parent, start, end);
}

void tree_view::on_drop()
{
  if (!m_mdl.is_layer(m_idx_menu)) return;
  auto lr(m_mdl.get_layer(m_idx_menu));

  QMessageBox dlg(QApplication::activeWindow());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setIcon(QMessageBox::Question);
  auto drop(dlg.addButton("drop", QMessageBox::AcceptRole));
  dlg.addButton("cancel", QMessageBox::RejectRole);
  dlg.setText(QString("do you want to drop %1?").arg(lr->get_string()));
  dlg.exec();
  if (dlg.clickedButton() != static_cast<QAbstractButton*>(drop)) return;

  qRegisterMetaType<provider_ptr>("provider_ptr");
  task_drop* tsk(new task_drop(lr));
  connect(tsk, SIGNAL(signal_refresh(provider_ptr)), this, SLOT(on_refresh(provider_ptr)));
  emit signal_task(std::shared_ptr<task>(tsk));
}

void tree_view::on_paste_rows()
{
  auto lr_copy(m_lrs_copy.front());
  if (!m_mdl.is_layer(m_idx_menu)) return;
  auto lr_paste(m_mdl.get_layer(m_idx_menu));
  if (lr_copy->get_levels() != lr_paste->get_levels()) return;

  dialog_insert dlg(QApplication::activeWindow(), lr_copy, lr_paste);
  if (dlg.exec() != QDialog::Accepted) return;
  task_insert* tsk(new task_insert(lr_copy, lr_paste, dlg.get_items(), dlg.ccw(), dlg.view()));
  emit signal_task(std::shared_ptr<task>(tsk));
}

void tree_view::on_paste_layers()
{
  if (!m_mdl.is_provider(m_idx_menu)) return;
  auto pvd(m_mdl.get_provider(m_idx_menu));

  dialog_create dlg
    ( QApplication::activeWindow()
    , m_lrs_copy.size() == 1? m_lrs_copy.front()->get_string(): QString("%1 layers").arg(m_lrs_copy.size())
    , pvd->is_database()
    );
  if (dlg.exec() != QDialog::Accepted) return;

  qRegisterMetaType<provider_ptr>("provider_ptr");
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  task_create* tsk(new task_create(m_lrs_copy, pvd, dlg.sql(), dlg.view()));
  connect
    ( tsk, SIGNAL(signal_sql(provider_ptr, std::vector<std::string>))
    , this, SLOT(emit_sql(provider_ptr, std::vector<std::string>))
    );
  connect
    ( tsk, SIGNAL(signal_refresh(provider_ptr))
    , this, SLOT(on_refresh(provider_ptr))
    );
  emit signal_task(std::shared_ptr<task>(tsk));
}

void tree_view::on_sql_console()
{
  if (!m_mdl.is_provider(m_idx_menu)) return;
  emit signal_sql(m_mdl.get_provider(m_idx_menu), std::vector<std::string>());
}

void tree_view::on_use_projection()
{
  if (!m_mdl.is_layer(m_idx_menu)) return;
  auto lr(m_mdl.get_layer(m_idx_menu));

  projection pj;
  if (lr->try_pj(pj))
    emit signal_proj(pj);
  else
  {
    qRegisterMetaType<projection>("projection");
    task_proj* tsk(new task_proj(lr));
    connect(tsk, SIGNAL(signal_proj(projection)), this, SLOT(emit_proj(projection)));
    emit signal_task(std::shared_ptr<task>(tsk));
  }
}

void tree_view::on_snap_to_pixels()
{
  if (!m_mdl.is_layer(m_idx_menu)) return;
  auto lr(m_mdl.get_layer(m_idx_menu));

  qRegisterMetaType<projection>("projection");
  task_scale* tsk(new task_scale(lr));
  connect(tsk, SIGNAL(signal_scale(double, projection)), this, SLOT(emit_scale(double, projection)));
  emit signal_task(std::shared_ptr<task>(tsk));
}

void tree_view::on_zoom_to_fit()
{
  if (!m_mdl.is_layer(m_idx_menu)) return;
  auto lr(m_mdl.get_layer(m_idx_menu));

  brig::boost::box box;
  projection pj;
  if (lr->try_view(box, pj))
    emit signal_rect(box_to_rect(box), pj);
  else
  {
    qRegisterMetaType<projection>("projection");
    task_extent* tsk(new task_extent(lr));
    connect(tsk, SIGNAL(signal_rect(QRectF, projection)), this, SLOT(emit_rect(QRectF, projection)));
    emit signal_task(std::shared_ptr<task>(tsk));
  }
}

void tree_view::on_refresh(provider_ptr pvd)
{
  struct provider_allocator : task_connect::provider_allocator {
    provider_ptr pvd;
    explicit provider_allocator(provider_ptr pvd_)
      : pvd(pvd_)
      {}
    QString get_string() override
      { return pvd->get_string(); }
    provider_ptr allocate() override
      { return pvd; }
  }; // provider_allocator

  std::vector<std::shared_ptr<task_connect::provider_allocator>> allocators;
  allocators.push_back(std::make_shared<provider_allocator>(pvd));
  connect_by(allocators);
}

void tree_view::on_refresh()
{
  if (!m_mdl.is_provider(m_idx_menu)) return;
  auto pvd(m_mdl.get_provider(m_idx_menu));
  on_refresh(pvd);
}
