// Andrew Naplavkov

#include <brig/boost/as_binary.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/correct.hpp>
#include "rowset_ccw.h"

bool rowset_ccw::fetch(std::vector<brig::variant>& row)
{
  if (!m_rs->fetch(row)) return false;
  for (size_t i(0); i < m_cols.size(); ++i)
    if (row[m_cols[i]].type() == typeid(brig::blob_t))
      row[m_cols[i]] = brig::boost::as_binary(brig::boost::correct(brig::boost::geom_from_wkb(boost::get<brig::blob_t>(row[m_cols[i]]))));
  return true;
}
