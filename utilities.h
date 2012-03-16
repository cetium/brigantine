// Andrew Naplavkov

#ifndef UTILITIES_H
#define UTILITIES_H

#include <brig/boost/geometry.hpp>
#include <brig/proj/epsg.hpp>
#include <QApplication>
#include <QCursor>
#include <QRectF>
#include <QString>
#include <Qt>
#include "frame.h"

const int SignalInterval = 750;
const int TimeOut = 3000;

QRectF box_to_rect(const brig::boost::box& box);
brig::boost::box rect_to_box(const QRectF& rect);

QRectF proj_to_pixel(const QRectF& rect, const frame& fr);
QRectF pixel_to_proj(const QRectF& rect, const frame& fr);
QRectF transform(const QRectF& rect, const brig::proj::epsg& from, const brig::proj::epsg& to);
QPointF transform(const QPointF& point, const brig::proj::epsg& from, const brig::proj::epsg& to);

brig::proj::epsg get_epsg(int code);
inline brig::proj::epsg latlon()  { return get_epsg(4326); }
QRectF world(const brig::proj::epsg& epsg);

QString rich_text(const QString& icon, const QString& text);
QString to_string(size_t num);
void show_message(const char* msg);

struct wait_cursor {
  wait_cursor()
  {
    #ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    #endif
  }

  virtual ~wait_cursor()
  {
    #ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
    #endif
  }
}; // wait_cursor

#endif // UTILITIES_H
