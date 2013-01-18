// Andrew Naplavkov

#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QWidget>
#include <vector>
#include "frame.h"
#include "connection_link.h"
#include "layer_link.h"
#include "map_thread.h"
#include "task.h"

class map_view : public QWidget {
  Q_OBJECT

  map_thread m_trd;
  frame m_view_fr, m_pix_fr;
  QPixmap m_pix;
  std::vector<layer_link> m_lrs;
  QPointF m_press_center;
  QPoint m_press_event;

  void zoom(double zoom_factor);
  void scroll(int delta_x, int delta_y);
  void render()  { m_trd.render(m_lrs, m_view_fr); }

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent*) override  { emit signal_coords(""); }

private slots:
  void emit_idle()  { emit signal_idle(); }
  void emit_process(QString msg)  { emit signal_process(msg); }
  void emit_start()  { emit signal_start(); }
  void on_process(frame fr, QImage image);

public slots:
  void on_layers(std::vector<layer_link> lrs);
  void on_proj(brig::proj::shared_pj pj);
  void on_view(QRectF rect, brig::proj::shared_pj pj);
  void on_task(std::shared_ptr<task> tsk);

signals:
  void signal_task(std::shared_ptr<task> tsk);
  void signal_start();
  void signal_process(QString msg);
  void signal_idle();
  void signal_active(brig::proj::shared_pj pj);
  void signal_coords(QString msg);

public:
  explicit map_view(QWidget* parent);
}; // map_view

#endif // MAP_VIEW_H
