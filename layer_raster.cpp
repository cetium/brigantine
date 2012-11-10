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
  using namespace std;

  auto tbl(get_connection()->get_table_definition(m_raster.levels[level].geometry));
  if (!m_raster.levels[level].raster.query_expression.empty())
    tbl.columns.push_back(m_raster.levels[level].raster);

  for (auto cnd(begin(m_raster.levels[level].query_conditions)); cnd != end(m_raster.levels[level].query_conditions); ++cnd)
  {
    auto col(tbl[cnd->name]);
    if (!cnd->query_expression.empty()) col->query_expression = cnd->query_expression;
    col->query_value = cnd->query_value;
  }
  return tbl;
}

void layer_raster::reset_table_definitions()
{
  for (size_t level(0); level < m_raster.levels.size(); ++level)
    get_connection()->reset_table_definition(m_raster.levels[level].geometry);
}

bool layer_raster::is_writable()
{
  return !m_raster.levels.empty() && m_raster.levels[0].raster.query_expression.empty();
}

layer* layer_raster::reg(connection_link dbc, std::vector<std::string>& sql)
{
  auto raster(dbc->fit_to_reg(m_raster));
  dbc->reg(raster, sql);
  return new layer_raster(dbc, raster);
}

void layer_raster::unreg(std::vector<std::string>& sql)
{
  get_connection()->unreg(m_raster, sql);
}

size_t layer_raster::get_level(const frame& fr)
{
  using namespace std;

  const brig::proj::epsg epsg_rast(get_epsg());
  const brig::proj::epsg epsg_fr(fr.get_epsg());
  double scale(1);
  if (int(epsg_rast) == int(epsg_fr))
    scale = fr.scale();
  else
  {
    const QRectF rect1(pixel_to_proj(QRectF(QPointF(), fr.size()), fr).intersect(world(epsg_fr)));
    if (!rect1.isValid()) return m_raster.levels.size() - 1;
    const QRectF rect2(transform(rect1, epsg_fr, epsg_rast).intersect(world(epsg_rast)));
    if (!rect2.isValid()) return m_raster.levels.size() - 1;
    const double zoom_factor(std::min<>(rect2.width() / rect1.width(), rect2.height() / rect1.height()));
    scale = fr.scale() * zoom_factor;
  }

  vector<double> dists;
  for (size_t level(0); level < m_raster.levels.size(); ++level)
    dists.push_back(pow(m_raster.levels[level].resolution_x - scale, 2) + pow(m_raster.levels[level].resolution_y - scale, 2));
  return distance(begin(dists), min_element(begin(dists), end(dists)));
}

std::string layer_raster::get_raster_column(size_t level) const
{
  return m_raster.levels[level].raster.name;
}

bool layer_raster::has_spatial_index(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  return tbl.rtree(m_raster.levels[level].geometry.qualifier) != 0;
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
  if (row.size() < 2 || row[0].type() != typeid(brig::blob_t) || row[1].type() != typeid(brig::blob_t)) return;
  const brig::blob_t g(boost::get<brig::blob_t>(row[0]));
  const brig::blob_t r(boost::get<brig::blob_t>(row[1]));
  const QRectF rect_rast(box_to_rect(brig::boost::envelope(brig::boost::geom_from_wkb(g))));
  QImage img_rast;
  if (!img_rast.loadFromData(r.data(), uint(r.size()))) return;
  const brig::proj::epsg epsg_rast(get_epsg());
  const brig::proj::epsg epsg_fr(fr.get_epsg());

  if (int(epsg_rast) == int(epsg_fr))
    painter.drawImage(proj_to_pixel(rect_rast, fr).toAlignedRect(), img_rast);
  else
  {
    const QRectF rect_fr(transform(rect_rast, epsg_rast, epsg_fr));
    const QRect rect_fr_px(proj_to_pixel(fr.prepare_rect().intersect(rect_fr), fr).toAlignedRect());
    if (!rect_fr_px.isValid()) return;
    QImage img_fr(rect_fr_px.size(), QImage::Format_ARGB32_Premultiplied);
    for (int j(0); j < img_fr.height(); ++j)
      for (int i(0); i < img_fr.width(); ++i)
      {
        const QPointF point_fr(fr.pixel_to_proj(rect_fr_px.topLeft() + QPoint(i, j)));
        const QPointF point_rast(transform(point_fr, epsg_fr, epsg_rast));
        if (rect_rast.contains(point_rast))
        {
          const double dx((point_rast.x() - rect_rast.left()) / rect_rast.width());
          const double dy((point_rast.y() - rect_rast.top()) / rect_rast.height());
          QRgb rgb(img_rast.pixel(int(dx * img_rast.width()), int((1 - dy) * img_rast.height())));
          img_fr.setPixel(i, j, rgb);
        }
        else
          img_fr.setPixel(i, j, QColor(0, 0, 0, 0).rgba());
      }
    painter.drawImage(rect_fr_px, img_fr);
  }
}
