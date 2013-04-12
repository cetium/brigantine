// Andrew Naplavkov

#ifndef FRAME_H
#define FRAME_H

#include <brig/qt/frame.hpp>
#include <QRectF>
#include "projection.h"

class frame : public brig::qt::frame {
  projection m_pj;
public:
  frame()  {}
  frame(const QPointF& center, qreal scale, const QSize& size, const projection& pj);
  bool operator ==(const frame& r) const  { return brig::qt::frame::operator ==(r) && m_pj == r.m_pj; }
  bool operator !=(const frame& r) const  { return !operator ==(r); }
  projection get_pj() const  { return m_pj; }
  QRectF prepare_rect() const;
  QRectF proj_to_pixel(const QRectF& rect) const;
  QRectF pixel_to_proj(const QRectF& rect) const;
  frame transform(const projection& pj) const;
}; // frame

#endif // FRAME_H
