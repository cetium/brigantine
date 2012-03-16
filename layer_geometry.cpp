// Andrew Naplavkov

#include <brig/database/threaded_rowset.hpp>
#include <brig/proj/transform_wkb.hpp>
#include <brig/qt/draw.hpp>
#include <exception>
#include "layer_geometry.h"
#include "utilities.h"

QString layer_geometry::get_string()
{
  QString str;
  if (!m_id.schema.empty()) str += QString::fromUtf8(m_id.schema.c_str()) + ".";
  str += QString::fromUtf8(m_id.name.c_str()) + "." + QString::fromUtf8(m_id.qualifier.c_str());
  return str;
}

brig::database::table_definition layer_geometry::get_table_definition(size_t)
{
  return m_tbl.columns.empty()? get_connection()->get_table_definition(m_id): m_tbl;
}

std::shared_ptr<brig::database::rowset> layer_geometry::attributes(const frame& fr)
{
  auto tbl(get_table_definition(0));
  tbl.box_filter_column = m_id.qualifier;
  tbl.box_filter = prepare_box(fr);
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name != m_id.qualifier) tbl.select_columns.push_back(tbl.columns[i].name);
  tbl.rows = int(limit());
  return get_connection()->get_table(tbl);
}

std::shared_ptr<brig::database::rowset> layer_geometry::drawing(const frame& fr, bool limited)
{
  class reproject : public brig::database::rowset {
    std::shared_ptr<rowset> m_rs;
    brig::proj::epsg m_from, m_to;
  public:
    reproject(std::shared_ptr<rowset> rs, const brig::proj::epsg& from, const brig::proj::epsg& to) : m_rs(rs), m_from(from), m_to(to)  {}
    virtual std::vector<std::string> columns()  { return m_rs->columns(); }
    virtual bool fetch(std::vector<brig::database::variant>& row)
      {
        if (!m_rs->fetch(row)) return false;
        try  { if (!row.empty() && row[0].type() == typeid(brig::blob_t)) brig::proj::transform_wkb(boost::get<brig::blob_t>(row[0]), m_from, m_to); }
        catch (const std::exception&)  { row[0] = brig::database::null_t(); }
        return true;
      }
  }; // reproject

  auto tbl(get_table_definition(0));
  tbl.box_filter_column = m_id.qualifier;
  tbl.box_filter = prepare_box(fr);
  tbl.select_columns.push_back(m_id.qualifier);
  if (limited) tbl.rows = int(limit());
  auto rs(get_connection()->get_table(tbl));
  auto pj(get_epsg());
  return int(fr.get_epsg()) == int(pj)? rs: std::make_shared<brig::database::threaded_rowset>(std::make_shared<reproject>(rs, pj, fr.get_epsg()));
}

void layer_geometry::draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter)
{
  try
  {
    if (!row.empty() && row[0].type() == typeid(brig::blob_t))
      brig::qt::draw( boost::get<brig::blob_t>(row[0]), fr, painter );
  }
  catch (const std::exception&)  {}
}
