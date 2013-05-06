// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QIcon>
#include <QMessageBox>
#include <QPointF>
#include "transformer.h"
#include "utilities.h"

QRectF box_to_rect(const brig::boost::box& box)
{
  return QRectF(QPointF(box.min_corner().get<0>(), box.min_corner().get<1>()), QPointF(box.max_corner().get<0>(), box.max_corner().get<1>()));
}

brig::boost::box rect_to_box(const QRectF& rect)
{
  return brig::boost::box(brig::boost::point(rect.left(), rect.top()), brig::boost::point(rect.right(), rect.bottom()));
}

projection get_pj(const brig::column_def& col)
{
  return col.epsg < 0? projection(col.proj): projection(col.epsg);
}

projection latlon()
{
  return projection(4326);
}

QRectF world(const projection& pj)
{
  transformer tr(latlon(), pj);
  return tr.transform(QRectF(QPointF(-180, -90), QPointF(180, 90)));
}

QString rich_text(const QString& icon, const QString& txt, bool icon_suffix)
{
  if (icon_suffix)
    return txt + " <img src=\"" + icon + "\" width=\"16\" height=\"16\"/> ";
  else
    return " <img src=\"" + icon + "\" width=\"16\" height=\"16\"/> " + txt;
}

QString limited_text(QString txt, bool suffix)
{
  const int TextLimit = 40;
  if (txt.size() > TextLimit)
  {
    if (suffix) txt = "..." + txt.right(TextLimit);
    else txt = txt.left(TextLimit) + "...";
  }
  return txt;
}

void show_message(const char* msg)
{
  QMessageBox dlg;
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setIcon(QMessageBox::Information);
  dlg.setText(QString::fromUtf8(msg));
  dlg.exec();
}

std::string get_table_name(const std::string& tbl, size_t level)
{
  std::string str(tbl);
  if (level > 0) str += "_" + brig::string_cast<char>(level);
  return str;
}
