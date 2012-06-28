// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <cmath>
#include <QImage>
#include <vector>
#include "layer_raster.h"
#include "utilities.h"

brig::database::table_definition layer_raster::get_table_definition(size_t level)
{
  auto tbl(get_connection()->get_table_definition(m_raster.levels[level].geometry));
  if (!m_raster.levels[level].raster.query_expression.empty())
  {
    brig::database::column_definition col;
    col.name =  m_raster.levels[level].raster.name;
    col.query_expression = m_raster.levels[level].raster.query_expression;
    col.type = brig::database::Blob;
    tbl.columns.push_back(col);
  }

  bool create_index(true);
  for (auto idx(std::begin(tbl.indexes)); idx != std::end(tbl.indexes); ++idx)
    if ( brig::database::Spatial == idx->type
      && 1 == idx->columns.size()
      && idx->columns.front() == m_raster.levels[level].geometry.qualifier
       )
       create_index = false;

  if (create_index)
  {
    brig::database::index_definition idx;
    idx.type = brig::database::Spatial;
    idx.columns.push_back(m_raster.levels[level].geometry.qualifier);
    tbl.indexes.push_back(idx);
  }

  for (size_t i(0); i < m_raster.levels[level].query_values.size(); ++i)
  {
    auto val(m_raster.levels[level].query_values[i]);
    auto col(std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [&](const brig::database::column_definition& col_){ return col_.name == val.name; }));
    if (col != std::end(tbl.columns))
    {
      if (!val.query_expression.empty()) col->query_expression = val.query_expression;
      col->query_value = val.query_value;
    }
  }
  return tbl;
}

bool layer_raster::is_writable()
{
  return !m_raster.levels.empty() && m_raster.levels[0].raster.query_expression.empty();
}

layer* layer_raster::create_result(connection_link dbc, const std::string& tbl, std::vector<std::string>& sql)
{
  brig::database::raster_pyramid raster(m_raster);
  for (size_t level(0); level < raster.levels.size(); ++level)
    raster.levels[level].geometry.name = get_table_name(tbl, level);
  dbc->create_result(raster, sql);
  return new layer_raster(dbc, raster);
}

void layer_raster::drop_meta(std::vector<std::string>& sql)
{
  get_connection()->drop_meta(m_raster, sql);
}

size_t layer_raster::get_level(const frame& fr) const
{
  std::vector<double> dists;
  for (size_t level(0); level < m_raster.levels.size(); ++level)
    dists.push_back(pow(m_raster.levels[level].resolution.get<0>() - fr.scale(), 2) + pow(m_raster.levels[level].resolution.get<1>() - fr.scale(), 2));
  return std::distance(std::begin(dists), std::min_element(std::begin(dists), std::end(dists)));
}

std::string layer_raster::get_raster_column(size_t level) const
{
  return m_raster.levels[level].raster.name;
}

std::shared_ptr<brig::database::rowset> layer_raster::attributes(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
  {
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
    else if (tbl.columns[i].name != get_raster_column(level))
      tbl.query_columns.push_back(tbl.columns[i].name);
  }
  tbl.query_rows = int(limit());
  return get_connection()->select(tbl);
}

std::shared_ptr<brig::database::rowset> layer_raster::drawing(const frame& fr, bool limited)
{
  if (int(fr.get_epsg()) != int(get_epsg())) throw std::runtime_error("projection error");
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_columns.push_back(m_raster.levels[level].geometry.qualifier);
  tbl.query_columns.push_back(get_raster_column(level));
  if (limited) tbl.query_rows = int(limit());
  return get_connection()->select(tbl);
}

void layer_raster::draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter)
{
  try
  {
    if (row.size() < 2 || row[0].type() != typeid(brig::blob_t) || row[1].type() != typeid(brig::blob_t)) return;
    brig::blob_t g(boost::get<brig::blob_t>(row[0])), r(boost::get<brig::blob_t>(row[1]));
    QRectF rect(proj_to_pixel(box_to_rect(brig::boost::envelope(brig::boost::geom_from_wkb(g))), fr));
    QImage img;
    if (img.loadFromData(r.data(), uint(r.size()))) painter.drawImage(rect.toAlignedRect(), img);
  }
  catch (const std::exception&)  {}
}
