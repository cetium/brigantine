// Andrew Naplavkov

#include <algorithm>
#include "connection.h"
#include "layer.h"
#include "tree_item.h"

tree_item::tree_item(const tree_item* parent, connection_link dbc) : m_parent(parent)  { m_var.setValue(dbc); }
tree_item::tree_item(const tree_item* parent, layer_link lr) : m_parent(parent)  { m_var.setValue(lr); }

connection_link tree_item::get_connection() const
{
  if (m_var.userType() == qMetaTypeId<connection_link>())
    return qvariant_cast<connection_link>(m_var);
  else if (m_var.userType() == qMetaTypeId<layer_link>())
    return qvariant_cast<layer_link>(m_var)->get_connection();
  else
    return connection_link();
}

layer_link tree_item::get_layer() const
{
  if (m_var.userType() == qMetaTypeId<layer_link>())
    return qvariant_cast<layer_link>(m_var);
  else
    return layer_link();
}

QString tree_item::get_string() const
{
  if (m_var.userType() == qMetaTypeId<connection_link>())
    return qvariant_cast<connection_link>(m_var)->get_string();
  else if (m_var.userType() == qMetaTypeId<layer_link>())
    return qvariant_cast<layer_link>(m_var)->get_string();
  else
    return QString();
}

void tree_item::check(size_t order)
{
  if (m_var.userType() != qMetaTypeId<layer_link>()) return;
  auto lr(qvariant_cast<layer_link>(m_var));
  lr.m_checked = !lr.m_checked;
  lr.m_order = order;
  m_var.setValue(lr);
}

int tree_item::position() const
{
  if (!m_parent) return -1;
  auto p(std::find_if(std::begin(m_parent->m_children), std::end(m_parent->m_children), [&](const std::unique_ptr<tree_item>& i){ return i.get() == this; }));
  if (p == std::end(m_parent->m_children)) return -1;
  return std::distance(std::begin(m_parent->m_children), p);
}
