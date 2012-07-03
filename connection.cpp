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
  auto iter(m_tables.find(tbl));
  if (iter == std::end(m_tables))
  {
    m_tables.insert(std::pair<brig::database::identifier, brig::database::table_definition>(tbl, brig::database::connection<true>::get_table_definition(tbl)));
    iter = m_tables.find(tbl);
  }
  return iter->second;
}

void connection::reset_table_definition(const brig::database::identifier& tbl)
{
  QMutexLocker locker(&m_mutex);
  auto iter(m_tables.find(tbl));
  if (iter != std::end(m_tables)) m_tables.erase(iter);
}

brig::database::column_definition* connection::get_column_definition_ptr(const brig::database::identifier& id)
{
  auto iter(m_tables.find(id));
  if (iter == std::end(m_tables))
  {
    m_tables.insert(std::pair<brig::database::identifier, brig::database::table_definition>(id, brig::database::connection<true>::get_table_definition(id)));
    iter = m_tables.find(id);
  }
  return iter->second[id.qualifier];
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
