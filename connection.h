// Andrew Naplavkov

#ifndef CONNECTION_H
#define CONNECTION_H

#include <brig/boost/geometry.hpp>
#include <brig/database/connection.hpp>
#include <map>
#include <memory>
#include <QMutex>
#include <QString>

class connection : public brig::database::connection<true>
{
  struct compare {
    bool operator()(const brig::database::identifier& a, const brig::database::identifier& b) const  { return a.schema < b.schema || a.name < b.name; }
  }; // compare

  const QString m_str;
  QMutex m_mutex;
  std::map<brig::database::identifier, brig::database::table_definition, compare> m_tables;

  std::vector<brig::database::column_definition>::iterator get_column_definition_iterator(const brig::database::identifier& id);

public:
  connection(std::shared_ptr<brig::database::command_allocator> allocator, QString str);
  QString get_string() const  { return m_str; }
  QString get_icon();

  brig::database::table_definition get_table_definition(const brig::database::identifier& id);
  void reset_table_definition(const brig::database::identifier& id);

  brig::database::column_definition get_column_definition(const brig::database::identifier& id);
  void set_mbr(const brig::database::identifier& id, const brig::boost::box& box);
}; // connection

#endif // CONNECTION_H
