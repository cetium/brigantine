// Andrew Naplavkov

#include <brig/proj/transform_wkb.hpp>
#include "reproject.h"

bool reproject::fetch(std::vector<brig::database::variant>& row)
{
  if (!m_rs->fetch(row)) return false;
  for (size_t i(0); i < m_map.size(); ++i)
  {
    if (row[m_map[i].column].type() == typeid(brig::blob_t))
      brig::proj::transform_wkb(boost::get<brig::blob_t>(row[m_map[i].column]), m_map[i].pj_from, m_map[i].pj_to);
  }
  return true;
}
