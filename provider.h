// Andrew Naplavkov

#ifndef PROVIDER_H
#define PROVIDER_H

#include <brig/database/provider.hpp>
#include <brig/provider.hpp>
#include <map>
#include <memory>
#include <QMutex>
#include <QString>

class provider : public brig::provider
{
  struct compare {
    bool operator()(const brig::identifier& a, const brig::identifier& b) const
      { return a.schema < b.schema || (a.schema == b.schema && a.name < b.name); }
  }; // compare

  std::unique_ptr<brig::provider> m_pvd;
  const QString m_str;
  const QString m_icon;
  QMutex m_mtx;
  std::map<brig::identifier, brig::table_def, compare> m_tables;

  brig::database::provider<true>* get_database()  { return dynamic_cast<brig::database::provider<true>*>(m_pvd.get()); }

public:
  provider(brig::provider* pvd, QString str, QString icon);

  std::vector<brig::identifier> get_tables() override  { return m_pvd->get_tables(); }
  std::vector<brig::identifier> get_geometry_layers() override  { return m_pvd->get_geometry_layers(); }
  std::vector<brig::pyramid_def> get_raster_layers() override  { return m_pvd->get_raster_layers(); }
  brig::table_def get_table_def(const brig::identifier& tbl) override;
  brig::boost::box get_extent(const brig::table_def& tbl) override  { return m_pvd->get_extent(tbl); }
  std::shared_ptr<brig::rowset> select(const brig::table_def& tbl) override  { return m_pvd->select(tbl); }
  bool is_readonly() override  { return m_pvd->is_readonly(); }
  brig::table_def fit_to_create(const brig::table_def& tbl) override  { return m_pvd->fit_to_create(tbl); }
  void create(const brig::table_def& tbl) override  { m_pvd->create(tbl); }
  void drop(const brig::table_def& tbl) override  { m_pvd->drop(tbl); }
  brig::pyramid_def fit_to_reg(const brig::pyramid_def& raster) override  { return m_pvd->fit_to_reg(raster); }
  void reg(const brig::pyramid_def& raster) override  { m_pvd->reg(raster); }
  void unreg(const brig::pyramid_def& raster) override  { m_pvd->unreg(raster); }
  std::shared_ptr<brig::inserter> get_inserter(const brig::table_def& tbl) override  { return m_pvd->get_inserter(tbl); }

  bool is_database()  { return get_database() != 0; }
  std::shared_ptr<brig::database::command> get_command()  { return get_database()->get_command(); }
  void create(const brig::table_def& tbl, std::vector<std::string>& sql)  { get_database()->create(tbl, sql); }
  void reg(const brig::pyramid_def& raster, std::vector<std::string>& sql)  { get_database()->reg(raster, sql); }

  QString get_string() const  { return m_str; }
  QString get_icon() const  { return m_icon; }
  void reset_table_def();
  void reset_table_def(const brig::identifier& tbl);
  bool try_table_def(const brig::identifier& tbl, brig::table_def& tbl_def);
  void set_extent(const brig::identifier& col, const brig::boost::box& box);
}; // provider

#endif // PROVIDER_H
