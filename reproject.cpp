// Andrew Naplavkov

#include <brig/proj/transform_wkb.hpp>
#include "reproject.h"

bool reproject::fetch(std::vector<brig::variant>& row)
{
  if (!m_rs->fetch(row)) return false;
  for (size_t i(0); i < m_items.size(); ++i)
  {
    if (row[m_items[i].column].type() == typeid(brig::blob_t))
      brig::proj::transform_wkb(boost::get<brig::blob_t>(row[m_items[i].column]), m_items[i].pj_from, m_items[i].pj_to);
  }
  return true;
}
