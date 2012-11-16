// Andrew Naplavkov

#ifndef UTILITIES_H
#define UTILITIES_H

#include <brig/boost/geometry.hpp>
#include <brig/proj/shared_pj.hpp>
#include <QApplication>
#include <QCursor>
#include <QRectF>
#include <QString>
#include <Qt>
#include <string>
#include "frame.h"

const int BatchInterval = 3000;
const int SignalInterval = 750;
const int TimeOut = 3000;

QRectF box_to_rect(const brig::boost::box& box);
brig::boost::box rect_to_box(const QRectF& rect);

QRectF proj_to_pixel(const QRectF& rect, const frame& fr);
QRectF pixel_to_proj(const QRectF& rect, const frame& fr);
QRectF transform(const QRectF& rect, const brig::proj::shared_pj& from, const brig::proj::shared_pj& to);
QPointF transform(const QPointF& point, const brig::proj::shared_pj& from, const brig::proj::shared_pj& to);

brig::proj::shared_pj get_pj(int epsg);
int get_epsg(const brig::proj::shared_pj& pj);
inline brig::proj::shared_pj latlon()  { return get_pj(4326); }
QRectF world(const brig::proj::shared_pj& pj);

QString rich_text(const QString& icon, const QString& text, bool icon_suffix);
void show_message(const char* msg);
std::string get_table_name(const std::string& tbl, size_t level);

struct wait_cursor {
  wait_cursor()
  {
    #ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    #endif
  }

  ~wait_cursor()
  {
    #ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
    #endif
  }
}; // wait_cursor

#endif // UTILITIES_H
