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
  const auto def = m_pvd->get_table_def(tbl);
  {
    QMutexLocker lck(&m_mtx);
    if (m_tables.find(tbl) == std::end(m_tables))
      m_tables[tbl] = def;
  }
  return def;
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

bool provider::try_table_def(const brig::identifier& tbl, brig::table_def& def)
{
  QMutexLocker lck(&m_mtx);
  if (m_tables.find(tbl) == std::end(m_tables))
    return false;
  def = m_tables[tbl];
  return true;
}

void provider::set_extent(const brig::identifier& col, const brig::boost::box& box)
{
  QMutexLocker lck(&m_mtx);
  if (m_tables.find(col) != std::end(m_tables))
    m_tables[col][col.qualifier]->query_value = brig::boost::as_binary(box);
}
