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
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) == std::end(m_tables))
    m_tables[tbl] = m_pvd->get_table_def(tbl);
  return m_tables[tbl];
}

void provider::reset_table_def(const brig::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) != std::end(m_tables))
    m_tables.erase(tbl);
}

bool provider::try_table_def(const brig::identifier& tbl, brig::table_def& def)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) == std::end(m_tables))
    return false;
  def = m_tables[tbl];
  return true;
}

void provider::set_mbr(const brig::identifier& col, const brig::boost::box& box)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(col) != std::end(m_tables))
    m_tables[col][col.qualifier]->query_value = brig::boost::as_binary(box);
}
