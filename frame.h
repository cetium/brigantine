// Andrew Naplavkov

#ifndef FRAME_H
#define FRAME_H

#include <brig/proj/shared_pj.hpp>
#include <brig/qt/frame.hpp>
#include <QRectF>

class frame : public brig::qt::frame {
  brig::proj::shared_pj m_pj;
public:
  frame()  {}
  frame(const QPointF& center, qreal scale, const QSize& size, const brig::proj::shared_pj& pj);
  bool operator ==(const frame& r) const  { return brig::qt::frame::operator ==(r) && m_pj == r.m_pj; }
  bool operator !=(const frame& r) const  { return !operator ==(r); }
  brig::proj::shared_pj get_pj() const  { return m_pj; }
  QRectF prepare_rect() const;
}; // frame

#endif // FRAME_H
