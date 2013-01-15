// Andrew Naplavkov

#include <algorithm>
#include <brig/proj/transform.hpp>
#include <brig/string_cast.hpp>
#include <cmath>
#include <map>
#include <QIcon>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <QPointF>
#include <QSizeF>
#include <stdexcept>
#include <vector>
#include "utilities.h"

QRectF box_to_rect(const brig::boost::box& box)
{
  return QRectF(QPointF(box.min_corner().get<0>(), box.min_corner().get<1>()), QPointF(box.max_corner().get<0>(), box.max_corner().get<1>()));
}

brig::boost::box rect_to_box(const QRectF& rect)
{
  return brig::boost::box(brig::boost::point(rect.left(), rect.top()), brig::boost::point(rect.right(), rect.bottom()));
}

QRectF proj_to_pixel(const QRectF& rect, const frame& fr)
{
  return QRectF(fr.proj_to_pixel(rect.bottomLeft()), fr.proj_to_pixel(rect.topRight()));
}

QRectF pixel_to_proj(const QRectF& rect, const frame& fr)
{
  return QRectF(fr.pixel_to_proj(rect.bottomLeft()), fr.pixel_to_proj(rect.topRight()));
}

bool not_huge(double val)  { return -HUGE_VAL < val && val < HUGE_VAL; }

QRectF transform(const QRectF& rect, const brig::proj::shared_pj& from, const brig::proj::shared_pj& to)
{
  using namespace std;

  if (from == to) return rect;

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

  brig::proj::transform(points_xy.data(), long(points_xy.size() / 2), from, to);

  vector<double> xs, ys;
  for (size_t i(0); i < points_xy.size(); i += 2)
  {
    if (not_huge(points_xy[i]))
      xs.push_back(points_xy[i]);
    if (not_huge(points_xy[i + 1]))
      ys.push_back(points_xy[i + 1]);
  }

  if (xs.empty() || ys.empty()) throw runtime_error("projection error");

  return QRectF
    ( QPointF(*min_element(begin(xs), end(xs)), *min_element(begin(ys), end(ys)))
    , QPointF(*max_element(begin(xs), end(xs)), *max_element(begin(ys), end(ys)))
    );
}

QPointF transform(const QPointF& point, const brig::proj::shared_pj& from, const brig::proj::shared_pj& to)
{
  double xy[2];
  xy[0] = point.x();
  xy[1] = point.y();
  brig::proj::transform(xy, 1, from, to);
  return QPointF(xy[0], xy[1]);
}

static QMutex s_proj_mtx;
static std::map<int, brig::proj::shared_pj> s_proj_map;

brig::proj::shared_pj get_pj(const brig::column_definition& col)
{
  if (col.epsg < 0)
    return brig::proj::shared_pj(col.proj);
  else
  {
    QMutexLocker locker(&s_proj_mtx);
    auto iter(s_proj_map.find(col.epsg));
    if (iter == std::end(s_proj_map))
    {
      brig::proj::shared_pj pj(col.epsg);
      s_proj_map.insert(std::pair<int, brig::proj::shared_pj>(col.epsg, pj));
      iter = s_proj_map.find(col.epsg);
    }
    return iter->second;
  }
}

int get_epsg(const brig::proj::shared_pj& pj)
{
  QMutexLocker locker(&s_proj_mtx);
  for (auto iter(std::begin(s_proj_map)); iter != std::end(s_proj_map); ++iter)
    if (pj == iter->second) return iter->first;
  return -1;
}

brig::proj::shared_pj latlon()
{
  brig::column_definition col;
  col.epsg = 4326;
  return get_pj(col);
}

QRectF world(const brig::proj::shared_pj& pj)
{
  return transform(QRectF(QPointF(-180, -90), QPointF(180, 90)), latlon(), pj);
}

QString rich_text(const QString& icon, const QString& text, bool icon_suffix)
{
  if (icon_suffix)
    return text + " <img src=\"" + icon + "\" width=\"16\" height=\"16\"/> ";
  else
    return " <img src=\"" + icon + "\" width=\"16\" height=\"16\"/> " + text;
}

void show_message(const char* msg)
{
  QMessageBox dlg;
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setText(QString::fromUtf8(msg));
  dlg.exec();
}

std::string get_table_name(const std::string& tbl, size_t level)
{
  std::string str(tbl);
  if (level > 0) str += "_" + brig::string_cast<char>(level);
  return str;
}
