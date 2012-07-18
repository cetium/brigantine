// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/as_binary.hpp>
#include <QMutexLocker>
#include <utility>
#include "connection.h"

connection::connection(std::shared_ptr<brig::database::command_allocator> allocator, QString str)
  : brig::database::connection<true>(allocator)
  , m_str(str)
{}

QString connection::get_icon()
{
  using namespace brig::database;
  switch (get_command()->system())
  {
  default: return ":/res/anonymous.png";
  case CUBRID: return ":/res/cubrid.png";
  case DB2: return ":/res/db2.png";
  case Informix: return ":/res/informix.png";
  case MS_SQL: return ":/res/ms_sql.png";
  case MySQL: return ":/res/mysql.png";
  case Oracle: return ":/res/oracle.png";
  case Postgres: return ":/res/postgres.png";
  case SQLite: return ":/res/sqlite.png";
  }
}

brig::database::table_definition connection::get_table_definition(const brig::database::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) == std::end(m_tables))
    m_tables[tbl] = brig::database::connection<true>::get_table_definition(tbl);
  return m_tables[tbl];
}

void connection::reset_table_definition(const brig::database::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  if (m_tables.find(tbl) != std::end(m_tables)) m_tables.erase(tbl);
}

brig::database::column_definition* connection::get_column_definition_ptr(const brig::database::identifier& id)
{

  if (m_tables.find(id) == std::end(m_tables))
    m_tables[id] = brig::database::connection<true>::get_table_definition(id);
  return m_tables[id][id.qualifier];
}

brig::database::column_definition connection::get_column_definition(const brig::database::identifier& col)
{
  QMutexLocker locker(&m_mutex);
  return *get_column_definition_ptr(col);
}

void connection::set_mbr(const brig::database::identifier& col, const brig::boost::box& box)
{
  QMutexLocker locker(&m_mutex);
  get_column_definition_ptr(col)->query_value = brig::boost::as_binary(box);
}
