// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/as_binary.hpp>
#include <QMutexLocker>
#include "connection.h"

connection::connection(brig::connection* dbc, QString str, QString icon)
  : m_dbc(dbc), m_str(str), m_icon(icon)
{
}

brig::table_definition connection::get_table_definition(const brig::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) == std::end(m_tables))
    m_tables[tbl] = m_dbc->get_table_definition(tbl);
  return m_tables[tbl];
}

void connection::reset_table_definition(const brig::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) != std::end(m_tables))
    m_tables.erase(tbl);
}

bool connection::try_table_definition(const brig::identifier& tbl, brig::table_definition& def)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) == std::end(m_tables))
    return false;
  def = m_tables[tbl];
  return true;
}

void connection::set_mbr(const brig::identifier& col, const brig::boost::box& box)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(col) != std::end(m_tables))
    m_tables[col][col.qualifier]->query_value = brig::boost::as_binary(box);
}
