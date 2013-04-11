// Andrew Naplavkov

#include <algorithm>
#include <cfloat>
#include <exception>
#include <iterator>
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
#include "task_rendering.h"
#include "utilities.h"

const double ZoomInFactor = 0.8f;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

map_view::map_view(QWidget* parent) : QWidget(parent), m_sync(false), m_tasks(0)
{
  setFocusPolicy(Qt::ClickFocus);
#ifndef QT_NO_CURSOR
  setCursor(Qt::ArrowCursor);
  setMouseTracking(true);
#endif
  m_time.start();
  try
  {
    m_view_fr = frame(QPointF(0, 0), 1, QSize(360, 180), latlon());
    on_home();
  }
  catch (const std::exception&)  {}
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
    if (m_press_event != event->pos()) render();
  }
  catch (const std::exception&)  {}
}

void map_view::paintEvent(QPaintEvent*)
{
  if (m_front.isNull() || m_view_fr.get_pj() != m_front_fr.get_pj()) return;
  QPainter painter(this);
  if (m_view_fr == m_front_fr)
    painter.drawPixmap(QPoint(), m_front);
  else
  {
    try
    {
      QPointF offset(m_view_fr.proj_to_pixel(m_front_fr.pixel_to_proj(QPointF()))); // m_view_fr.pj == m_front_fr.pj
      const double scale(m_front_fr.scale() / m_view_fr.scale());

      painter.save();
      painter.translate(offset);
      painter.scale(scale, scale);
      QRectF exposed = painter.matrix().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
      painter.drawPixmap(exposed, m_front, exposed);
      painter.restore();
    }
    catch (const std::exception&)  {}
  }
}

void map_view::sync()
{
  if (m_sync || m_view_fr != m_back_fr) return;
  m_front_fr = m_back_fr;
  m_front = m_back.copy();
  m_sync = true;
  update();
}

void map_view::emit_rendering(const std::vector<layer_ptr>& lrs)
{
  qRegisterMetaType<frame>("frame");
  qRegisterMetaType<QImage>("QImage");
  for (auto itr(std::begin(lrs)); itr != std::end(lrs); ++itr)
  {
    task_rendering* tsk = new task_rendering(*itr);
    tsk->set_frame(m_view_fr);
    connect(this, SIGNAL(signal_cancel()), tsk, SLOT(on_cancel()));
    connect(tsk, SIGNAL(signal_image(frame, QImage)), this, SLOT(on_image(frame, QImage)));
    connect(tsk, SIGNAL(signal_finished(QString)), this, SLOT(on_finished(QString)));
    emit signal_task(std::shared_ptr<task>(tsk));
    ++m_tasks;
  }
  emit signal_progress();
  m_time.restart();
}

void map_view::render()
{
  emit signal_cancel();
  m_back_fr = m_view_fr;
  QImage img(m_back_fr.size(), QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&img);
  painter.eraseRect(img.rect());
  m_back = QPixmap::fromImage(img);
  m_sync = false;
  if (m_lrs.empty()) sync();
  else emit_rendering(m_lrs);
}

void map_view::on_image(frame fr, QImage img)
{
  if (m_back_fr != fr) return;
  QPainter painter(&m_back);
  painter.setCompositionMode(QPainter::CompositionMode_Darken);
  painter.drawImage(0, 0, img);
  m_sync = false;
  if (m_time.elapsed() < RenderingInterval) return;
  sync();
  m_time.restart();
}

void map_view::on_finished(QString)
{
  --m_tasks;
  if (m_tasks > 0) return;
  emit signal_idle();
  sync();
}

void map_view::on_layers(std::vector<layer_ptr> lrs)
{
  using namespace std;
  auto cmp = [](const layer_ptr& a, const layer_ptr& b){ return (void*)a.operator ->() < (void*)b.operator ->(); };
  vector<layer_ptr> lrs_old(m_lrs), lrs_new(lrs), lrs_old_diff, lrs_new_diff;
  sort(begin(lrs_old), end(lrs_old), cmp);
  sort(begin(lrs_new), end(lrs_new), cmp);
  set_difference(begin(lrs_old), end(lrs_old), begin(lrs_new), end(lrs_new), back_inserter(lrs_old_diff), cmp);
  set_difference(begin(lrs_new), end(lrs_new), begin(lrs_old), end(lrs_old), back_inserter(lrs_new_diff), cmp);
  m_lrs = lrs;
  if (!lrs_old_diff.empty()) render();
  else if (!lrs_new_diff.empty()) emit_rendering(lrs_new_diff);
}

void map_view::on_proj(brig::proj::shared_pj pj)
{
  try
  {
    if (projPJ(pj) == 0 || pj == m_view_fr.get_pj()) return;
    m_view_fr = transform(m_view_fr, pj);
    render();
    emit signal_proj(m_view_fr.get_pj());
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
    if (pj != m_view_fr.get_pj()) center = transform(center, pj, m_view_fr.get_pj());
    const frame view_fr(center, scale, m_view_fr.size(), m_view_fr.get_pj());
    if (view_fr == m_view_fr) return;
    m_view_fr = view_fr;
    render();
    emit signal_proj(m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::on_home()
{
  auto pj(latlon());
  on_rect(world(pj), pj);
}

void map_view::on_scale(double scale, brig::proj::shared_pj pj)
{
  try
  {
    if (projPJ(pj) == 0) return;
    QPointF center(m_view_fr.center());
    if (pj != m_view_fr.get_pj()) center = transform(center, m_view_fr.get_pj(), pj);
    const frame view_fr(center, scale, m_view_fr.size(), pj);
    if (view_fr == m_view_fr) return;
    m_view_fr = view_fr;
    render();
    emit signal_proj(m_view_fr.get_pj());
  }
  catch (const std::exception&)  {}
}

void map_view::on_task(std::shared_ptr<task> tsk)
{
  tsk->set_frame(m_view_fr);
  emit signal_task(tsk);
}
