// Andrew Naplavkov

#include <algorithm>
#include <exception>
#include <iterator>
#include <QIcon>
#include "layer.h"
#include "layer_geometry.h"
#include "layer_raster.h"
#include "provider.h"
#include "tree_model.h"

tree_model::tree_model(QObject* parent) : QAbstractItemModel(parent), m_root(0, provider_ptr()), m_order(0)
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
  return int(parent_itm->m_children.size());
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
  using namespace std;

  if (!is_provider(idx)) return;
  tree_item* pvd_itm(static_cast<tree_item*>(idx.internalPointer()));
  bool render(find_if
    ( begin(pvd_itm->m_children)
    , end(pvd_itm->m_children)
    , [&](const std::unique_ptr<tree_item>& lr_itm){ return lr_itm->get_layer().m_checked; }
    ) != end(pvd_itm->m_children));

  emit signal_disconnect(pvd_itm->get_provider());

  beginRemoveRows(QModelIndex(), idx.row(), idx.row());
  auto iter(begin(m_root.m_children));
  advance(iter, idx.row());
  m_root.m_children.erase(iter);
  endRemoveRows();

  if (render) emit_layers();
}

void tree_model::on_connected(provider_ptr pvd, std::vector<layer_ptr> lrs)
{
  using namespace std;

  auto old_itm(find_if(begin(m_root.m_children), end(m_root.m_children), [&](const std::unique_ptr<tree_item>& itm)
    { return itm->get_provider()->get_string() == pvd->get_string(); }));

  if (old_itm == end(m_root.m_children))
  {
    unique_ptr<tree_item> new_itm(new tree_item(&m_root, pvd));
    for (auto lr(begin(lrs)); lr != end(lrs); ++lr)
      new_itm->m_children.emplace_back(new tree_item(new_itm.get(), *lr));

    beginInsertRows(QModelIndex(), int(m_root.m_children.size()), int(m_root.m_children.size()));
    m_root.m_children.push_back(move(new_itm));
    endInsertRows();
  }
  else
  {
    bool render(false);
    for (auto old_iter(begin((*old_itm)->m_children)); old_iter != end((*old_itm)->m_children); ++old_iter)
    {
      auto old_lr((*old_iter)->get_layer());
      auto new_lr(find_if(begin(lrs), end(lrs), [&](const layer_ptr& lr)
        { return lr->get_string() == old_lr->get_string(); }));
      if (new_lr != end(lrs))
      {
        new_lr->m_checked = old_lr.m_checked;
        new_lr->m_order = old_lr.m_order;
      }
      else if (old_lr.m_checked)
        render = true;
    }

    QModelIndex idx(createIndex(distance(begin(m_root.m_children), old_itm), 0, (*old_itm).get()));

    beginRemoveRows(idx, 0, int((*old_itm)->m_children.size() - 1));
    (*old_itm)->m_children.clear();
    endRemoveRows();

    beginInsertRows(idx, 0, int((*old_itm)->m_children.size() - 1));
    (*old_itm)->m_var.setValue(pvd);
    for (auto lr(begin(lrs)); lr != end(lrs); ++lr)
      (*old_itm)->m_children.emplace_back(new tree_item((*old_itm).get(), *lr));
    endInsertRows();

    if (render) emit_layers();
  }
}
