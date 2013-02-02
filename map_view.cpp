// Andrew Naplavkov

#include <algorithm>
#include <cfloat>
#include <exception>
#include <memory>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QString>
#include <QtGlobal>
#include <QWheelEvent>
#include <QWidget>
#include "map_view.h"
#include "utilities.h"

const double ZoomInFactor = 0.8f;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

map_view::map_view(QWidget* parent) : QWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);
#ifndef QT_NO_CURSOR
  setCursor(Qt::ArrowCursor);
  setMouseTracking(true);
#endif

  qRegisterMetaType<QImage>("QImage");
  qRegisterMetaType<frame>("frame");
  connect(&m_trd, SIGNAL(signal_start()), this, SLOT(emit_start()));
  connect(&m_trd, SIGNAL(signal_process(frame, QImage)), this, SLOT(on_process(frame, QImage)));
  connect(&m_trd, SIGNAL(signal_process(QString)), this, SLOT(emit_process(QString)));
  connect(&m_trd, SIGNAL(signal_idle()), this, SLOT(emit_idle()));

  try
  {
    m_view_fr = frame(QPointF(0, 0), 1, QSize(360, 180), latlon());
    on_rect(world(m_view_fr.get_pj()), m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::paintEvent(QPaintEvent*)
{
  if (m_pix.isNull() || !(m_view_fr.get_pj() == m_pix_fr.get_pj())) return;

  QPainter painter(this);
  if (m_view_fr == m_pix_fr)
    painter.drawPixmap(QPoint(), m_pix);
  else
  {
    try
    {
      QPointF offset(m_view_fr.proj_to_pixel(m_pix_fr.pixel_to_proj(QPointF()))); // m_view_fr.pj == m_pix_fr.pj
      const double scale(m_pix_fr.scale() / m_view_fr.scale());

      painter.save();
      painter.translate(offset);
      painter.scale(scale, scale);
      QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
      painter.drawPixmap(exposed, m_pix, exposed);
      painter.restore();
    }
    catch (const std::exception&)  {}
  }
}

void map_view::resizeEvent(QResizeEvent* event)
{
  if (m_view_fr.size() == event->size()) return;
  try
  {
    m_view_fr = frame(m_view_fr.center(), m_view_fr.scale(), event->size(), m_view_fr.get_pj());
    render();
  }
  catch (const std::exception&)  {}
}

void map_view::on_process(frame fr, QImage image)
{
  if (!(m_view_fr == fr)) return;
  m_pix_fr = m_view_fr;
  m_pix = QPixmap::fromImage(image);
  update();
}

void map_view::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
  case Qt::Key_Plus: zoom(ZoomInFactor); break;
  case Qt::Key_Minus: zoom(ZoomOutFactor); break;
  case Qt::Key_Left: scroll(-ScrollStep, 0); break;
  case Qt::Key_Right: scroll(+ScrollStep, 0); break;
  case Qt::Key_Down: scroll(0, -ScrollStep); break;
  case Qt::Key_Up: scroll(0, +ScrollStep); break;
  case Qt::Key_Home:
    try
    {
      brig::proj::shared_pj pj(latlon());
      on_proj(pj);
      on_rect(world(pj), pj);
    }
    catch (const std::exception&)  {}
    break;
  default: QWidget::keyPressEvent(event);
  }
}

void map_view::wheelEvent(QWheelEvent* event)
{
  try
  {
    const QPointF pos(m_view_fr.pixel_to_proj(event->pos()));
    const int num_degrees(event->delta() / 8);
    const double num_steps(num_degrees / 15.);
    const double zoom_factor(pow(ZoomInFactor, num_steps));
    const double scale(m_view_fr.scale() * zoom_factor);
    QPointF center(m_view_fr.center() - QPointF((pos - m_view_fr.center()) * (zoom_factor - 1)));
    m_view_fr = frame(center, scale, m_view_fr.size(), m_view_fr.get_pj());
    update();
    render();
  }
  catch (const std::exception&)  {}
}

void map_view::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_press_center = m_view_fr.center();
    m_press_event = event->pos();
  }
}

void map_view::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    try
    {
      frame view_fr(m_press_center, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_pj());
      const QPointF press(view_fr.pixel_to_proj(m_press_event));
      const QPointF pos(view_fr.pixel_to_proj(event->pos()));
      m_view_fr = frame(m_press_center + press - pos, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_pj());
      update();
    }
    catch (const std::exception&)  {}
  }
  else if (projPJ(m_view_fr.get_pj()) != 0)
  {
    QString x, y, res;
    try
    {
      const QPointF point(transform(m_view_fr.pixel_to_proj(event->pos()), m_view_fr.get_pj(), latlon()));
      if (point.x() > -180 && point.x() < 180 && point.y() > -90 && point.y() < 90)
      {
        x.setNum(point.x(), 'f', 4);
        y.setNum(point.y(), 'f', 4);
        res = x + " , " + y;
      }
    }
    catch (const std::exception&)  {}
    emit signal_coords(res);
  }
}

void map_view::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() != Qt::LeftButton) return;
  try
  {
    frame view_fr(m_press_center, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_pj());
    const QPointF press(view_fr.pixel_to_proj(m_press_event));
    const QPointF pos(view_fr.pixel_to_proj(event->pos()));
    m_view_fr = frame(m_press_center + press - pos, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_pj());
    update();
    if (m_press_event != event->pos()) render();
  }
  catch (const std::exception&)  {}
}

void map_view::zoom(double zoom_factor)
{
  try
  {
    m_view_fr = frame(m_view_fr.center(), m_view_fr.scale() * zoom_factor, m_view_fr.size(), m_view_fr.get_pj());
    update();
    render();
  }
  catch (const std::exception&)  {}
}

void map_view::scroll(int delta_x, int delta_y)
{
  try
  {
    m_view_fr = frame(m_view_fr.center() + QPointF(delta_x * m_view_fr.scale(), delta_y * m_view_fr.scale()), m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_pj());
    update();
    render();
  }
  catch (const std::exception&)  {}
}

void map_view::on_layers(std::vector<layer_ptr> lrs)
{
  m_lrs = lrs;
  update();
  render();
  emit signal_active(m_view_fr.get_pj());
}

void map_view::on_proj(brig::proj::shared_pj pj)
{
  try
  {
    if (projPJ(pj) == 0 || pj == m_view_fr.get_pj()) return;
    m_view_fr = transform(m_view_fr, pj);
    m_pix = QPixmap();
    update();
    render();
    emit signal_active(m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::on_rect(QRectF rect, brig::proj::shared_pj pj)
{
  try
  {
    if (!rect.isValid() || projPJ(pj) == 0 || size().width() == 0 || size().height() == 0) return;
    const QRectF rect_fr_px(proj_to_pixel(transform(rect, pj, m_view_fr.get_pj()), m_view_fr));
    const double zoom_factor(std::max<>(rect_fr_px.width() / qreal(size().width()), rect_fr_px.height() / qreal(size().height())));
    const double scale(m_view_fr.scale() * zoom_factor * (1. + DBL_EPSILON)); // "zoom to fit" workaround
    QPointF center(rect.center());
    if (!(pj == m_view_fr.get_pj())) center = transform(center, pj, m_view_fr.get_pj());
    const frame view_fr(center, scale, m_view_fr.size(), m_view_fr.get_pj());
    if (view_fr == m_view_fr) return;
    m_view_fr = view_fr;
    update();
    render();
    emit signal_active(m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::on_scale(double scale, brig::proj::shared_pj pj)
{
  try
  {
    if (projPJ(pj) == 0) return;
    QPointF center(m_view_fr.center());
    if (!(pj == m_view_fr.get_pj()))
    {
      center = transform(center, m_view_fr.get_pj(), pj);
      m_pix = QPixmap();
    }
    const frame view_fr(center, scale, m_view_fr.size(), pj);
    if (view_fr == m_view_fr) return;
    m_view_fr = view_fr;
    update();
    render();
    emit signal_active(m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::on_task(std::shared_ptr<task> tsk)
{
  tsk->set_frame(m_view_fr);
  emit signal_task(tsk);
}
