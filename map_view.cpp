// Andrew Naplavkov

#include <algorithm>
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
#include "task_attributes.h"
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
  connect(&m_trd, SIGNAL(signal_start()), this, SLOT(on_start()));
  connect(&m_trd, SIGNAL(signal_process(frame, QImage)), this, SLOT(on_process(frame, QImage)));
  connect(&m_trd, SIGNAL(signal_process(QString)), this, SLOT(on_process(QString)));
  connect(&m_trd, SIGNAL(signal_idle()), this, SLOT(on_idle()));

  try
  {
    m_view_fr = frame(m_view_fr.center(), m_view_fr.scale(), m_view_fr.size(), latlon());
    set_view(world(m_view_fr.get_epsg()), m_view_fr.get_epsg());
  }
  catch (const std::exception&)  {}
}

void map_view::paintEvent(QPaintEvent*)
{
  if (m_pix.isNull() || int(m_view_fr.get_epsg()) != int(m_pix_fr.get_epsg())) return;

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
  m_view_fr = frame(m_view_fr.center(), m_view_fr.scale(), event->size(), m_view_fr.get_epsg());
  render();
}

void map_view::on_process(const frame& fr, const QImage& image)
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
      brig::proj::epsg pj(latlon());
      set_proj(pj);
      set_view(world(pj), pj);
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
    m_view_fr = frame(center, scale, m_view_fr.size(), m_view_fr.get_epsg());
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
      frame view_fr(m_press_center, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_epsg());
      const QPointF press(view_fr.pixel_to_proj(m_press_event));
      const QPointF pos(view_fr.pixel_to_proj(event->pos()));
      m_view_fr = frame(m_press_center + press - pos, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_epsg());
      update();
    }
    catch (const std::exception&)  {}
  }
  else if (int(m_view_fr.get_epsg()) >= 0)
  {
    QString x, y, res;
    try
    {
      const QPointF point(transform(m_view_fr.pixel_to_proj(event->pos()), m_view_fr.get_epsg(), latlon()));
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
    frame view_fr(m_press_center, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_epsg());
    const QPointF press(view_fr.pixel_to_proj(m_press_event));
    const QPointF pos(view_fr.pixel_to_proj(event->pos()));
    m_view_fr = frame(m_press_center + press - pos, m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_epsg());
    update();
    if (m_press_event != event->pos()) render();
  }
  catch (const std::exception&)  {}
}

void map_view::zoom(double zoom_factor)
{
  try
  {
    m_view_fr = frame(m_view_fr.center(), m_view_fr.scale() * zoom_factor, m_view_fr.size(), m_view_fr.get_epsg());
    update();
    render();
  }
  catch (const std::exception&)  {}
}

void map_view::scroll(int delta_x, int delta_y)
{
  m_view_fr = frame(m_view_fr.center() + QPointF(delta_x * m_view_fr.scale(), delta_y * m_view_fr.scale()), m_view_fr.scale(), m_view_fr.size(), m_view_fr.get_epsg());
  update();
  render();
}

void map_view::set_layers(std::vector<layer_link> lrs)
{
  m_lrs = lrs;
  update();
  render();
  emit signal_scene(m_view_fr.get_epsg());
}

void map_view::set_view(const QRectF& rect, const brig::proj::epsg& pj)
{
  try
  {
    if (size().width() == 0 || size().height() == 0 || !rect.isValid()) return;
    const QRectF box(proj_to_pixel(transform(rect, pj, m_view_fr.get_epsg()), m_view_fr));
    const double zoom_factor(std::max<>(box.width() / size().width(), box.height() / size().height()));
    const double scale(m_view_fr.scale() * zoom_factor);
    const QPointF center(m_view_fr.pixel_to_proj(box.center()));

    frame view_fr(center, scale, m_view_fr.size(), m_view_fr.get_epsg());
    if (view_fr == m_view_fr) return;
    view_fr.prepare_rect();
    m_view_fr = view_fr;

    update();
    render();
    emit signal_scene(m_view_fr.get_epsg());
  }
  catch (const std::exception&)  {}
}

void map_view::set_proj(const brig::proj::epsg& pj)
{
  try
  {
    if (int(pj) == int(m_view_fr.get_epsg())) return;
    const QRectF rect1(pixel_to_proj(QRectF(QPointF(), m_view_fr.size()), m_view_fr).intersect(world(m_view_fr.get_epsg())));
    if (!rect1.isValid()) return;
    const QRectF rect2(transform(rect1, m_view_fr.get_epsg(), pj).intersect(world(pj)));
    if (!rect2.isValid()) return;
    const double zoom_factor(std::min<>(rect2.width() / rect1.width(), rect2.height() / rect1.height()));
    const double scale(m_view_fr.scale() * zoom_factor);
    const QPointF center(transform(m_view_fr.center(), m_view_fr.get_epsg(), pj));

    m_pix = QPixmap();
    m_view_fr = frame(center, scale, m_view_fr.size(), pj);

    update();
    render();
    emit signal_scene(m_view_fr.get_epsg());
  }
  catch (const std::exception&)  {}
}

void map_view::on_attributes(layer_link lr)
{
  qRegisterMetaType<connection_link>("connection_link");
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  task_attributes* tsk(new task_attributes(m_view_fr, lr));
  connect
    ( tsk, SIGNAL(signal_commands(connection_link, std::vector<std::string>))
    , this, SLOT(emit_commands(connection_link, std::vector<std::string>))
    );
  emit signal_task(std::shared_ptr<task>(tsk));
}
