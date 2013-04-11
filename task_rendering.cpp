// Andrew Naplavkov

#include <QColor>
#include <QEventLoop>
#include <QPainter>
#include <QTime>
#include <stdexcept>
#include "layer.h"
#include "provider.h"
#include "task_rendering.h"
#include "utilities.h"

QString task_rendering::get_string()
{
  return QString("rendering '%1'").arg(m_lr->get_string(true));
}

void task_rendering::emit_progress(QString wrn, size_t counter)
{
  if (wrn.isEmpty())
    emit signal_progress(QString("rows: %1").arg(counter));
  else
    emit signal_progress(QString("rows: %1 (%2)").arg(counter).arg(wrn));
}

void task_rendering::do_run(QEventLoop& loop)
{
  QString wrn;
  size_t counter(0);
  QTime time; time.start();
  if (!m_lr->has_spatial_index(m_fr)) wrn = "no index";
  QImage img(m_fr.size(), QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&img);
  painter.eraseRect(img.rect());
  painter.setPen(Qt::black);
  painter.setBrush(Qt::lightGray);
  std::vector<brig::variant> row;
  for (auto rs(m_lr->drawing(m_fr)); rs->fetch(row); ++counter)
  {
    m_lr->draw(row, m_fr, painter);
    if (time.elapsed() < RenderingInterval) continue;
    loop.processEvents();
    if (m_cancel) throw std::runtime_error("canceled");
    emit signal_image(m_fr, img.copy());
    emit_progress(wrn, counter);
    painter.eraseRect(img.rect());
    time.restart();
  }
  loop.processEvents();
  if (m_cancel) throw std::runtime_error("canceled");
  if (counter > 0) emit signal_image(m_fr, img.copy());
  emit_progress(wrn, counter);
}
