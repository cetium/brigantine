// Andrew Naplavkov

#ifndef UTILITIES_H
#define UTILITIES_H

#include <brig/boost/geometry.hpp>
#include <brig/column_def.hpp>
#include <QRectF>
#include <QString>
#include <string>
#include "projection.h"

const int BatchInterval = 3000;
const int RenderingInterval = 750;
const int TimeOut = 3000;

QRectF box_to_rect(const brig::boost::box& box);
brig::boost::box rect_to_box(const QRectF& rect);

projection get_pj(const brig::column_def& col);
projection latlon();
QRectF world(const projection& pj);

QString rich_text(const QString& icon, const QString& text, bool icon_suffix);
QString limited_text(QString txt, bool suffix);
void show_message(const char* msg);
std::string get_table_name(const std::string& tbl, size_t level);

#endif // UTILITIES_H
