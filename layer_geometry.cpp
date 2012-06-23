// Andrew Naplavkov

#include <brig/database/threaded_rowset.hpp>
#include <brig/qt/draw.hpp>
#include <exception>
#include <vector>
#include "layer_geometry.h"
#include "reproject.h"
#include "utilities.h"

layer_geometry::layer_geometry(connection_link dbc, const brig::database::identifier& id, const brig::database::table_definition& tbl)
  : layer(dbc), m_id(id), m_tbl(tbl)
{}

brig::database::table_definition layer_geometry::get_table_definition(size_t)
{
  return m_tbl.columns.empty()? get_connection()->get_table_definition(m_id): m_tbl;
}

layer* layer_geometry::create_result(connection_link dbc, const std::string& tbl, std::vector<std::string>&)
{
  brig::database::identifier id(m_id);
  id.name = tbl;
  dbc->create_result(id);
  return new layer_geometry(dbc, id);
}

std::shared_ptr<brig::database::rowset> layer_geometry::attributes(const frame& fr)
{
  auto tbl(get_table_definition(0));
  for (size_t i(0); i < tbl.columns.size(); ++i)
  {
    if (tbl.columns[i].name == m_id.qualifier)
      tbl.columns[i].query_condition = prepare_box(fr);
    else
      tbl.query_columns.push_back(tbl.columns[i].name);
  }
  tbl.query_rows = int(limit());
  return get_connection()->select(tbl);
}

std::shared_ptr<brig::database::rowset> layer_geometry::drawing(const frame& fr, bool limited)
{
  auto tbl(get_table_definition(0));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_id.qualifier)
      tbl.columns[i].query_condition = prepare_box(fr);
  tbl.query_columns.push_back(m_id.qualifier);
  if (limited) tbl.query_rows = int(limit());

  auto rs(get_connection()->select(tbl));
  reproject_item item;
  item.column = 0;
  item.pj_from = get_epsg();
  item.pj_to = fr.get_epsg();
  std::vector<reproject_item> items;
  if (int(item.pj_from) != int(item.pj_to)) items.push_back(item);
  return items.empty()? rs: std::make_shared<brig::database::threaded_rowset>(std::make_shared<reproject>(rs, items));
}

void layer_geometry::draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter)
{
  if (!row.empty() && row[0].type() == typeid(brig::blob_t))
    brig::qt::draw( boost::get<brig::blob_t>(row[0]), fr, painter );
}
