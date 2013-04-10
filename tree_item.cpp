// Andrew Naplavkov

#include <algorithm>
#include "layer.h"
#include "provider.h"
#include "tree_item.h"

tree_item::tree_item(const tree_item* parent, provider_ptr pvd) : m_parent(parent)  { m_var.setValue(pvd); }
tree_item::tree_item(const tree_item* parent, layer_ptr lr) : m_parent(parent)  { m_var.setValue(lr); }

provider_ptr tree_item::get_provider() const
{
  if (m_var.userType() == qMetaTypeId<provider_ptr>())
    return qvariant_cast<provider_ptr>(m_var);
  else if (m_var.userType() == qMetaTypeId<layer_ptr>())
    return qvariant_cast<layer_ptr>(m_var)->get_provider();
  else
    return provider_ptr();
}

layer_ptr tree_item::get_layer() const
{
  if (m_var.userType() == qMetaTypeId<layer_ptr>())
    return qvariant_cast<layer_ptr>(m_var);
  else
    return layer_ptr();
}

QString tree_item::get_string() const
{
  if (m_var.userType() == qMetaTypeId<provider_ptr>())
    return qvariant_cast<provider_ptr>(m_var)->get_string();
  else if (m_var.userType() == qMetaTypeId<layer_ptr>())
    return qvariant_cast<layer_ptr>(m_var)->get_string();
  else
    return QString();
}

void tree_item::check()
{
  if (m_var.userType() != qMetaTypeId<layer_ptr>()) return;
  auto lr(qvariant_cast<layer_ptr>(m_var));
  lr.m_checked = !lr.m_checked;
  m_var.setValue(lr);
}

int tree_item::position() const
{
  using namespace std;
  if (!m_parent) return -1;
  auto p(find_if(begin(m_parent->m_children), end(m_parent->m_children), [&](const std::unique_ptr<tree_item>& i){ return i.get() == this; }));
  if (p == end(m_parent->m_children)) return -1;
  return distance(begin(m_parent->m_children), p);
}
