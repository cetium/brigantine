// Andrew Naplavkov

#include <algorithm>
#include <brig/database/threaded_rowset.hpp>
#include <brig/qt/draw.hpp>
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

void layer_geometry::reset_table_definitions()
{
  if (m_tbl.columns.empty()) get_connection()->reset_table_definition(m_id);
}

layer* layer_geometry::reg(connection_link dbc, std::vector<std::string>&)
{
  auto tbl_from(get_table_definition(0));
  auto tbl_to(dbc->fit_to_create(tbl_from));

  for (size_t col(0), cols(std::min<>(tbl_from.columns.size(), tbl_to.columns.size())); col < cols; ++col)
    if (tbl_from.columns[col].name.compare(m_id.qualifier) == 0)
      tbl_to.id.qualifier = tbl_to.columns[col].name;

  return new layer_geometry(dbc, tbl_to.id);
}

bool layer_geometry::has_spatial_index(const frame&)
{
  auto tbl(get_table_definition(0));
  return tbl.rtree(m_id.qualifier) != 0;
}

std::shared_ptr<brig::database::rowset> layer_geometry::attributes(const frame& fr)
{
  auto tbl(get_table_definition(0));
  for (size_t i(0); i < tbl.columns.size(); ++i)
  {
    if (tbl.columns[i].name == m_id.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
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
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_columns.push_back(m_id.qualifier);
  if (limited) tbl.query_rows = int(limit());

  auto rs(get_connection()->select(tbl));
  reproject_item item;
  item.column = 0;
  item.pj_from = get_pj();
  item.pj_to = fr.get_pj();
  std::vector<reproject_item> items;
  if (!(item.pj_from == item.pj_to)) items.push_back(item);
  return items.empty()? rs: std::make_shared<brig::database::threaded_rowset>(std::make_shared<reproject>(rs, items));
}

void layer_geometry::draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter)
{
  if (!row.empty() && row[0].type() == typeid(brig::blob_t))
    brig::qt::draw( boost::get<brig::blob_t>(row[0]), fr, painter );
}
