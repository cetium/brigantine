// Andrew Naplavkov

#include "rowset_transform.h"

bool rowset_transform::fetch(std::vector<brig::variant>& row)
{
  if (!m_rs->fetch(row)) return false;
  for (size_t i(0); i < m_items.size(); ++i)
  {
    if (row[m_items[i].column].type() == typeid(brig::blob_t))
      m_items[i].tr.transform_wkb(boost::get<brig::blob_t>(row[m_items[i].column]));
  }
  return true;
}
