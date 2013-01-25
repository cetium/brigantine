// Andrew Naplavkov

#ifndef CONNECTION_H
#define CONNECTION_H

#include <brig/connection.hpp>
#include <brig/database/connection.hpp>
#include <map>
#include <memory>
#include <QMutex>
#include <QString>

class connection : public brig::connection
{
  struct compare {
    bool operator()(const brig::identifier& a, const brig::identifier& b) const
      { return a.schema < b.schema || (a.schema == b.schema && a.name < b.name); }
  }; // compare

  std::unique_ptr<brig::connection> m_dbc;
  const QString m_str;
  const QString m_icon;
  QMutex m_mutex;
  std::map<brig::identifier, brig::table_def, compare> m_tables;

  brig::database::connection<true>* get_database()  { return dynamic_cast<brig::database::connection<true>*>(m_dbc.get()); }

public:
  connection(brig::connection* dbc, QString str, QString icon);

  std::vector<brig::identifier> get_tables() override  { return m_dbc->get_tables(); }
  std::vector<brig::identifier> get_geometry_layers() override  { return m_dbc->get_geometry_layers(); }
  std::vector<brig::pyramid_def> get_raster_layers() override  { return m_dbc->get_raster_layers(); }
  brig::table_def get_table_def(const brig::identifier& tbl) override;
  brig::boost::box get_mbr(const brig::table_def& tbl, const std::string& col) override  { return m_dbc->get_mbr(tbl, col); }
  std::shared_ptr<brig::rowset> select(const brig::table_def& tbl) override  { return m_dbc->select(tbl); }
  brig::table_def fit_to_create(const brig::table_def& tbl) override  { return m_dbc->fit_to_create(tbl); }
  void create(const brig::table_def& tbl) override  { m_dbc->create(tbl); }
  void drop(const brig::table_def& tbl) override  { m_dbc->drop(tbl); }
  brig::pyramid_def fit_to_reg(const brig::pyramid_def& raster) override  { return m_dbc->fit_to_reg(raster); }
  void reg(const brig::pyramid_def& raster) override  { m_dbc->reg(raster); }
  void unreg(const brig::pyramid_def& raster) override  { m_dbc->unreg(raster); }
  std::shared_ptr<brig::inserter> get_inserter(const brig::table_def& tbl) override  { return m_dbc->get_inserter(tbl); }

  bool is_database()  { return get_database() != 0; }
  std::shared_ptr<brig::database::command> get_command()  { return get_database()->get_command(); }
  void create(const brig::table_def& tbl, std::vector<std::string>& sql)  { get_database()->create(tbl, sql); }
  void reg(const brig::pyramid_def& raster, std::vector<std::string>& sql)  { get_database()->reg(raster, sql); }

  QString get_string() const  { return m_str; }
  QString get_icon() const  { return m_icon; }
  void reset_table_def(const brig::identifier& tbl);
  bool try_table_def(const brig::identifier& tbl, brig::table_def& def);
  void set_mbr(const brig::identifier& col, const brig::boost::box& box);
}; // connection

#endif // CONNECTION_H
