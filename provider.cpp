// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/as_binary.hpp>
#include <QMutexLocker>
#include "provider.h"

provider::provider(brig::provider* pvd, QString str, QString icon)
  : m_pvd(pvd), m_str(str), m_icon(icon)
{
}

brig::table_def provider::get_table_def(const brig::identifier& tbl)
{
  {
    QMutexLocker lck(&m_mtx);
    if (m_tables.find(tbl) != std::end(m_tables))
      return m_tables[tbl];
  }
  const auto tbl_def = m_pvd->get_table_def(tbl);
  {
    QMutexLocker lck(&m_mtx);
    m_tables[tbl] = tbl_def;
  }
  return tbl_def;
}

void provider::reset_table_def()
{
  QMutexLocker lck(&m_mtx);
  m_tables.clear();
}

void provider::reset_table_def(const brig::identifier& tbl)
{
  QMutexLocker lck(&m_mtx);
  if (m_tables.find(tbl) != std::end(m_tables))
    m_tables.erase(tbl);
}

bool provider::try_table_def(const brig::identifier& tbl, brig::table_def& tbl_def)
{
  QMutexLocker lck(&m_mtx);
  if (m_tables.find(tbl) == std::end(m_tables))
    return false;
  tbl_def = m_tables[tbl];
  return true;
}

void provider::set_extent(const brig::identifier& col, const brig::boost::box& box)
{
  QMutexLocker lck(&m_mtx);
  if (m_tables.find(col) == std::end(m_tables))
    return;
  auto col_def = m_tables[col][col.qualifier];
  if (!col_def)
    return;
  col_def->query_value = brig::boost::as_binary(box);
}
