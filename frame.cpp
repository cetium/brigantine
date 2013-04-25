// Andrew Naplavkov

#include <algorithm>
#include <stdexcept>
#include "frame.h"
#include "transformer.h"
#include "utilities.h"

frame::frame(const QPointF& center, qreal scale, const QSize& size, const projection& pj)
  : brig::qt::frame(center, scale, size)
  , m_pj(pj)
{
  prepare_rect();
}

QRectF frame::prepare_rect() const
{
  const QRectF rect(pixel_to_proj(QRectF(QPointF(), size())).intersected(world(m_pj)));
  const QRectF total(proj_to_pixel(rect));
  if (total.width() < 0.5 && total.height() < 0.5) throw std::runtime_error("frame error");
  return rect;
}

QRectF frame::proj_to_pixel(const QRectF& rect) const
{
  return QRectF(brig::qt::frame::proj_to_pixel(rect.bottomLeft()), brig::qt::frame::proj_to_pixel(rect.topRight()));
}

QRectF frame::pixel_to_proj(const QRectF& rect) const
{
  return QRectF(brig::qt::frame::pixel_to_proj(rect.bottomLeft()), brig::qt::frame::pixel_to_proj(rect.topRight()));
}

frame frame::transform(const projection& pj) const
{
  transformer tr(m_pj, pj);
  const QRectF rect_from(prepare_rect());
  const QRectF rect_to(tr.transform(rect_from));
  const double zoom_factor(std::min<>(rect_to.width() / rect_from.width(), rect_to.height() / rect_from.height()));
  return frame(tr.transform(rect_from.center()), scale() * zoom_factor, size(), pj);
}
