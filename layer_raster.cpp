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

layer_raster::layer_raster(provider_ptr pvd, const brig::pyramid_def& raster)
  : layer(pvd), m_raster(raster)
{}

layer_raster::layer_raster(provider_ptr pvd, const brig::pyramid_def& raster, const std::vector<brig::table_def>& tbls)
  : layer(pvd), m_raster(raster), m_tbls(tbls)
{}

brig::table_def layer_raster::get_table_def(size_t lvl)
{
  using namespace std;

  auto tbl
    ( m_tbls.empty()
    ? get_provider()->get_table_def(m_raster.levels[lvl].geometry)
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

void layer_raster::reset_table_defs()
{
  if (m_tbls.empty())
    return;
  for (size_t lvl(0); lvl < m_raster.levels.size(); ++lvl)
    get_provider()->reset_table_def(m_raster.levels[lvl].geometry);
}

layer* layer_raster::fit(provider_ptr pvd)
{
  auto raster(pvd->fit_to_reg(m_raster));
  std::vector<brig::table_def> tbls;
  for (size_t lvl(0); lvl < get_levels(); ++lvl)
  {
    auto tbl(get_table_def(lvl));
    tbl.id.name = raster.levels[lvl].geometry.name;
    tbls.push_back(pvd->fit_to_create(tbl));
  }
  return new layer_raster(pvd, raster, tbls);
}

void layer_raster::reg()
{
  get_provider()->reg(m_raster);
}

void layer_raster::reg(std::vector<std::string>& sql)
{
  get_provider()->reg(m_raster, sql);
}

void layer_raster::unreg()
{
  get_provider()->unreg(m_raster);
}

size_t layer_raster::get_level(const frame& fr)
{
  const brig::proj::shared_pj pj_rast(get_pj());
  const brig::proj::shared_pj pj_fr(fr.get_pj());
  if (projPJ(pj_rast) == 0 || projPJ(pj_fr) == 0) return m_raster.levels.size() - 1;
  const double scale(pj_rast == pj_fr? fr.scale(): transform(fr, pj_rast).scale());
  return m_raster.snap_to_level(scale * scale);
}

std::string layer_raster::get_raster_column(size_t level) const
{
  return m_raster.levels[level].raster.name;
}

bool layer_raster::has_spatial_index(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_def(level));
  return tbl.rtree(m_raster.levels[level].geometry.qualifier) != 0;
}

std::shared_ptr<brig::rowset> layer_raster::attributes(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_def(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_rows = int(brig::PageSize);
  return get_provider()->select(tbl);
}

std::shared_ptr<brig::rowset> layer_raster::drawing(const frame& fr)
{
  size_t level(get_level(fr));
  auto tbl(get_table_def(level));
  for (size_t i(0); i < tbl.columns.size(); ++i)
    if (tbl.columns[i].name == m_raster.levels[level].geometry.qualifier)
      tbl.columns[i].query_value = prepare_box(fr);
  tbl.query_columns.push_back(m_raster.levels[level].geometry.qualifier);
  tbl.query_columns.push_back(get_raster_column(level));
  return get_provider()->select(tbl);
}

void layer_raster::draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter)
{
  if (row.size() < 2|| row[0].type() != typeid(brig::blob_t)) return;
  const brig::blob_t& g(boost::get<brig::blob_t>(row[0]));
  const QRectF rect_rast(box_to_rect(brig::boost::envelope(brig::boost::geom_from_wkb(g)))); // todo: rotated raster
  QImage img_rast;
  if (row[1].type() != typeid(brig::blob_t)) return;
  const brig::blob_t& r(boost::get<brig::blob_t>(row[1]));
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

double layer_raster::native_scale(const frame& fr)
{
  return m_raster.levels[get_level(fr)].resolution_x;
}
