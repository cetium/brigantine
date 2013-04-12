// Andrew Naplavkov

#include <algorithm>
#include <brig/proj/transform.hpp>
#include <brig/proj/transform_wkb.hpp>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <vector>
#include "transformer.h"

transformer::transformer(const projection& from, const projection& to)
{
  using namespace brig::proj;
  m_from = from.get_epsg() < 0? shared_pj(from.get_def()): shared_pj(from.get_epsg());
  m_to = to.get_epsg() < 0? shared_pj(to.get_def()): shared_pj(to.get_epsg());
}

bool transformer::not_huge(double val)
{
  return -HUGE_VAL < val && val < HUGE_VAL;
}

QRectF transformer::transform(const QRectF& rect) const
{
  using namespace std;

  if (m_from == m_to) return rect;

  static const int Partition = 32;
  const double step_x(rect.width() / double(Partition));
  const double step_y(rect.height() / double(Partition));
  vector<double> points_xy;
  for (int i(0); i <= Partition; ++i)
    for (int j(0); j <= Partition; ++j)
    {
      points_xy.push_back(rect.left() + i * step_x);
      points_xy.push_back(rect.top() + j * step_y);
    }

  brig::proj::transform(points_xy.data(), long(points_xy.size() / 2), m_from, m_to);

  vector<double> xs, ys;
  for (size_t i(0); i < points_xy.size(); i += 2)
  {
    if (not_huge(points_xy[i])) xs.push_back(points_xy[i]);
    if (not_huge(points_xy[i + 1])) ys.push_back(points_xy[i + 1]);
  }

  if (xs.empty() || ys.empty()) throw runtime_error("proj error");

  return QRectF
    ( QPointF(*min_element(begin(xs), end(xs)), *min_element(begin(ys), end(ys)))
    , QPointF(*max_element(begin(xs), end(xs)), *max_element(begin(ys), end(ys)))
    );
}

QPointF transformer::transform(const QPointF& point) const
{
  double xy[2];
  xy[0] = point.x();
  xy[1] = point.y();
  brig::proj::transform(xy, 1, m_from, m_to);
  return QPointF(xy[0], xy[1]);
}

void transformer::transform_wkb(brig::blob_t& wkb) const
{
  brig::proj::transform_wkb(wkb, m_from, m_to);
}
