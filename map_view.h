// Andrew Naplavkov

#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <memory>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QTime>
#include <QWidget>
#include <vector>
#include "frame.h"
#include "layer_ptr.h"
#include "provider_ptr.h"
#include "task.h"

class map_view : public QWidget {
  Q_OBJECT

  std::vector<layer_ptr> m_lrs;
  frame m_view_fr, m_front_fr, m_back_fr;
  QPixmap m_front, m_back;
  bool m_sync;
  QPointF m_press_center;
  QPoint m_press_event;
  size_t m_tasks;
  QTime m_time;

  void emit_rendering(const std::vector<layer_ptr>& lrs);
  void render();
  void sync();

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent*) override  { emit signal_coords(""); }

private slots:
  void on_image(frame fr, QImage img);
  void on_finished(QString);

public slots:
  void on_layers(std::vector<layer_ptr> lrs);
  void on_proj(brig::proj::shared_pj pj);
  void on_rect(QRectF rect, brig::proj::shared_pj pj);
  void on_home();
  void on_scale(double scale, brig::proj::shared_pj pj);
  void on_task(std::shared_ptr<task> tsk);

signals:
  void signal_cancel();
  void signal_proj(brig::proj::shared_pj pj);
  void signal_coords(QString msg);
  void signal_task(std::shared_ptr<task> tsk);
  void signal_progress();
  void signal_idle();

public:
  explicit map_view(QWidget* parent);
  ~map_view() override  { emit signal_cancel(); }
}; // map_view

#endif // MAP_VIEW_H
