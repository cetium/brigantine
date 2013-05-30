// Andrew Naplavkov

#include <QColor>
#include <QPainter>
#include <QTime>
#include <stdexcept>
#include "layer.h"
#include "provider.h"
#include "task_rendering.h"
#include "utilities.h"

QString task_rendering::get_string()
{
  return QString("rendering of '%1'").arg(m_lr->get_string(true));
}

void task_rendering::progress(QString wrn, size_t counter)
{
  if (wrn.isEmpty())
    task::progress(QString("rows: %1").arg(counter));
  else
    task::progress(QString("rows: %1 (%2)").arg(counter).arg(wrn));
}

void task_rendering::run_impl()
{
  QString wrn;
  size_t counter(0);
  QTime time; time.start();
  frame fr = get_frame();
  if (!m_lr->has_spatial_index(fr)) wrn = "no index";
  QImage img(fr.size(), QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&img);
  painter.eraseRect(img.rect());
  painter.setCompositionMode(QPainter::CompositionMode_Darken);
  painter.setPen(Qt::black);
  painter.setBrush(Qt::lightGray);
  std::vector<brig::variant> row;
  for (auto rs(m_lr->drawing(fr)); rs->fetch(row); ++counter)
  {
    m_lr->draw(row, fr, painter);
    if (time.elapsed() < RenderingInterval) continue;
    progress(wrn, counter);
    emit signal_image(fr, img.copy());
    painter.eraseRect(img.rect());
    time.restart();
  }
  progress(wrn, counter);
  if (counter > 0) emit signal_image(fr, img.copy());
}
