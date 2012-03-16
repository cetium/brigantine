// Andrew Naplavkov

#ifndef FRAME_H
#define FRAME_H

#include <brig/proj/epsg.hpp>
#include <brig/qt/frame.hpp>
#include <QRectF>

class frame : public brig::qt::frame {
  brig::proj::epsg m_pj;
public:
  frame()  {}
  frame(const QPointF& center, qreal scale, const QSize& size, const brig::proj::epsg& pj);
  bool operator==(const frame& r) const  { return brig::qt::frame::operator==(r) && int(m_pj) == int(r.m_pj); }
  brig::proj::epsg get_epsg() const  { return m_pj; }
  QRectF prepare_rect() const;
}; // frame

#endif // FRAME_H
