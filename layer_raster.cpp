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

QString layer_raster::get_string()
{
  QString str;
  if (!m_raster.id.schema.empty()) str += QString::fromUtf8(m_raster.id.schema.c_str()) + ".";
  str += QString::fromUtf8(m_raster.id.name.c_str()) + "." + QString::fromUtf8(m_raster.id.qualifier.c_str());
  return str;
}

brig::database::table_definition layer_raster::get_table_definition(size_t level)
{
  auto tbl(get_connection()->get_table_definition(get_geometry_column(level)));
  if (typeid(brig::database::column_definition) == m_raster.levels[level].raster_column.type())
    tbl.columns.push_back( boost::get<brig::database::column_definition>(m_raster.levels[level].raster_column) );
  tbl.select_sql_condition = m_raster.levels[level].sql_condition;
  tbl.select_parameters = m_raster.levels[level].parameters;
  return tbl;
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
  if (typeid(brig::database::column_definition) == m_raster.levels[level].raster_column.type())
    return boost::get<brig::database::column_definition>(m_raster.levels[level].raster_column).name;
  else
    return boost::get<std::string>(m_raster.levels[level].raster_column);
}

std::shared_ptr<brig::database::rowset> layer_raster::attributes(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  tbl.select_box_column = get_geometry_column(level).qualifier;
  tbl.select_box = prepare_box(fr);
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if ( tbl.columns[i].name != get_geometry_column(level).qualifier
      && tbl.columns[i].name != get_raster_column(level) )
      tbl.select_columns.push_back(tbl.columns[i].name);
  tbl.select_rows = int(limit());
  return get_connection()->get_table(tbl);
}

std::shared_ptr<brig::database::rowset> layer_raster::drawing(const frame& fr, bool limited)
{
  if (int(fr.get_epsg()) != int(get_epsg())) throw std::runtime_error("projection error");
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  tbl.select_box_column = get_geometry_column(level).qualifier;
  tbl.select_box = prepare_box(fr);
  tbl.select_columns.push_back(get_geometry_column(level).qualifier);
  tbl.select_columns.push_back(get_raster_column(level));
  if (limited) tbl.select_rows = int(limit());
  return get_connection()->get_table(tbl);
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
