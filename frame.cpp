// Andrew Naplavkov

#include <stdexcept>
#include "frame.h"
#include "utilities.h"

frame::frame(const QPointF& center, qreal scale, const QSize& size, const brig::proj::epsg& pj)
  : brig::qt::frame(center, scale, size)
  , m_pj(pj)
{}

QRectF frame::prepare_rect() const
{
  const QRectF rect(::pixel_to_proj(QRectF(QPointF(), size()), *this).intersected(world(m_pj)));
  const QRectF total(::proj_to_pixel(rect, *this));
  if (total.width() < 0.5 && total.height() < 0.5) throw std::runtime_error("frame error");
  return rect;
}
