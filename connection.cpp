// Andrew Naplavkov

#include <algorithm>
#include <stdexcept>
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
  default: return ":/anonymous.png";
  case DB2: return ":/db2.png";
  case MS_SQL: return ":/ms_sql.png";
  case MySQL: return ":/mysql.png";
  case Oracle: return ":/oracle.png";
  case Postgres: return ":/postgres.png";
  case SQLite: return ":/sqlite.png";
  }
}

brig::database::table_definition connection::get_table_definition(const brig::database::identifier& id)
{
  QMutexLocker locker(&m_mutex);
  auto iter(m_tables.find(id));
  if (iter == std::end(m_tables))
  {
    m_tables.insert(std::pair<brig::database::identifier, brig::database::table_definition>(id, brig::database::connection<true>::get_table_definition(id)));
    iter = m_tables.find(id);
  }
  return iter->second;
}

void connection::reset_table_definition(const brig::database::identifier& id)
{
  QMutexLocker locker(&m_mutex);
  auto iter(m_tables.find(id));
  if (iter != std::end(m_tables)) m_tables.erase(iter);
}

std::vector<brig::database::column_definition>::iterator connection::get_column_definition_iterator(const brig::database::identifier& id)
{
  auto iter(m_tables.find(id));
  if (iter == std::end(m_tables))
  {
    m_tables.insert(std::pair<brig::database::identifier, brig::database::table_definition>(id, brig::database::connection<true>::get_table_definition(id)));
    iter = m_tables.find(id);
  }

  auto is_column_name([&](const brig::database::column_definition& col){ return col.name == id.qualifier; });
  auto col(std::find_if(std::begin(iter->second.columns), std::end(iter->second.columns), is_column_name));
  if (col == std::end(iter->second.columns)) throw std::runtime_error("table error");
  return col;
}

brig::database::column_definition connection::get_column_definition(const brig::database::identifier& id)
{
  QMutexLocker locker(&m_mutex);
  return *get_column_definition_iterator(id);
}

void connection::set_mbr(const brig::database::identifier& id, const brig::boost::box& box)
{
  QMutexLocker locker(&m_mutex);
  get_column_definition_iterator(id)->mbr = box;
}
