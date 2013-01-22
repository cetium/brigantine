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

layer_raster::layer_raster(connection_link dbc, const brig::raster_pyramid& raster)
  : layer(dbc), m_raster(raster)
{}

layer_raster::layer_raster(connection_link dbc, const brig::raster_pyramid& raster, const std::vector<brig::table_definition>& tbls)
  : layer(dbc), m_raster(raster), m_tbls(tbls)
{}

brig::table_definition layer_raster::get_table_definition(size_t lvl)
{
  using namespace std;

  auto tbl
    ( m_tbls.empty()
    ? get_connection()->get_table_definition(m_raster.levels[lvl].geometry)
    : m_tbls[lvl]
    );
  if (!m_raster.levels[lvl].raster.query_expression.empty())
    tbl.columns.push_back(m_raster.levels[lvl].raster);

  for (auto cnd(begin(m_raster.levels[lvl].query_conditions)); cnd != end(m_raster.levels[lvl].query_conditions); ++cnd)
  {
    auto col(tbl[cnd->name]);
    if (!cnd->query_expression.empty()) col->query_expression = cnd->query_expression;
    col->query_value = cnd->query_value;
  }
  return tbl;
}

void layer_raster::reset_table_definitions()
{
  if (m_tbls.empty())
    return;
  for (size_t lvl(0); lvl < m_raster.levels.size(); ++lvl)
    get_connection()->reset_table_definition(m_raster.levels[lvl].geometry);
}

layer* layer_raster::fit(connection_link dbc)
{
  auto raster(dbc->fit_to_reg(m_raster));
  std::vector<brig::table_definition> tbls;
  for (size_t lvl(0); lvl < get_levels(); ++lvl)
  {
    auto tbl(get_table_definition(lvl));
    tbl.id.name = raster.levels[lvl].geometry.name;
    tbls.push_back(dbc->fit_to_create(tbl));
  }
  return new layer_raster(dbc, raster, tbls);
}

void layer_raster::reg()
{
  get_connection()->reg(m_raster);
}

void layer_raster::reg(std::vector<std::string>& sql)
{
  get_connection()->reg(m_raster, sql);
}

void layer_raster::unreg()
{
  get_connection()->unreg(m_raster);
}

size_t layer_raster::get_level(const frame& fr)
{
  using namespace std;

  const brig::proj::shared_pj pj_rast(get_pj());
  const brig::proj::shared_pj pj_fr(fr.get_pj());
  if (projPJ(pj_rast) == 0 || projPJ(pj_fr) == 0) return m_raster.levels.size() - 1;
  double scale(1);
  if (pj_rast == pj_fr)
    scale = fr.scale();
  else
  {
    const QRectF rect1(pixel_to_proj(QRectF(QPointF(), fr.size()), fr).intersected(world(pj_fr)));
    if (!rect1.isValid()) return m_raster.levels.size() - 1;
    const QRectF rect2(transform(rect1, pj_fr, pj_rast).intersected(world(pj_rast)));
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

std::shared_ptr<brig::rowset> layer_raster::attributes(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_rows = int(brig::PageSize);
  return get_connection()->select(tbl);
}

std::shared_ptr<brig::rowset> layer_raster::drawing(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_definition(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_columns.push_back(m_raster.levels[level].geometry.qualifier);
  tbl.query_columns.push_back(get_raster_column(level));
  return get_connection()->select(tbl);
}

void layer_raster::draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter)
{
  if (row.size() < 2|| row[0].type() != typeid(brig::blob_t)) return;
  const brig::blob_t g(boost::get<brig::blob_t>(row[0]));
  const QRectF rect_rast(box_to_rect(brig::boost::envelope(brig::boost::geom_from_wkb(g))));
  QImage img_rast;
  if (row[1].type() != typeid(brig::blob_t)) return;
  const brig::blob_t r(boost::get<brig::blob_t>(row[1]));
  if (!img_rast.loadFromData(r.data(), uint(r.size()))) return;
  const brig::proj::shared_pj pj_rast(get_pj());
  const brig::proj::shared_pj pj_fr(fr.get_pj());
  if (pj_rast == pj_fr)
    painter.drawImage(proj_to_pixel(rect_rast, fr).toAlignedRect(), img_rast);
  else
  {
    const QRectF rect_fr(transform(rect_rast, pj_rast, pj_fr));
    const QRect rect_fr_px(proj_to_pixel(fr.prepare_rect().intersected(rect_fr), fr).toAlignedRect());
    if (!rect_fr_px.isValid()) return;
    QImage img_fr(rect_fr_px.size(), QImage::Format_ARGB32_Premultiplied);
    for (int j(0); j < img_fr.height(); ++j)
      for (int i(0); i < img_fr.width(); ++i)
      {
        const QPointF point_fr(fr.pixel_to_proj(rect_fr_px.topLeft() + QPoint(i, j)));
        const QPointF point_rast(transform(point_fr, pj_fr, pj_rast));
        if (rect_rast.contains(point_rast))
        {
          const double dx((point_rast.x() - rect_rast.left()) / rect_rast.width());
          const double dy((point_rast.y() - rect_rast.top()) / rect_rast.height());
          auto pixel(img_rast.pixel(int(dx * img_rast.width()), int((1 - dy) * img_rast.height())));
          img_fr.setPixel(i, j, pixel);
        }
        else
          img_fr.setPixel(i, j, QColor(0, 0, 0, 0).rgba());
      }
    painter.drawImage(rect_fr_px, img_fr);
  }
}

double layer_raster::snap_to_pixels(const frame& fr)
{
  return m_raster.levels[get_level(fr)].resolution_x;
}
