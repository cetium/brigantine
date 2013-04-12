// Andrew Naplavkov

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <brig/blob_t.hpp>
#include <brig/proj/shared_pj.hpp>
#include <QPointF>
#include <QRectF>
#include "projection.h"

class transformer {
  brig::proj::shared_pj m_from, m_to;
  static bool not_huge(double val);
public:
  transformer()  {}
  transformer(const projection& from, const projection& to);
  bool to_latlong() const  { return m_to.is_latlong(); }
  QRectF transform(const QRectF& rect) const;
  QPointF transform(const QPointF& point) const;
  void transform_wkb(brig::blob_t& wkb) const;
}; // transformer

#endif // TRANSFORMER_H
