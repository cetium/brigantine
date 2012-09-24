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
  virtual void paintEvent(QPaintEvent* event);
  virtual void resizeEvent(QResizeEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void leaveEvent(QEvent*)  { emit signal_coords(""); }

private slots:
  void emit_commands(connection_link dbc, std::vector<std::string> sqls)  { emit signal_commands(dbc, sqls); }
  void emit_idle()  { emit signal_idle(); }
  void emit_process(QString msg)  { emit signal_process(msg); }
  void emit_start()  { emit signal_start(); }
  void on_process(frame fr, QImage image);

public slots:
  void on_attributes(layer_link lr);
  void on_layers(std::vector<layer_link> lrs);
  void on_proj(brig::proj::epsg pj);
  void on_view(QRectF rect, brig::proj::epsg pj);

signals:
  void signal_start();
  void signal_process(QString msg);
  void signal_idle();
  void signal_coords(QString msg);
  void signal_scene(brig::proj::epsg pj);
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
  void signal_task(std::shared_ptr<task> tsk);

public:
  explicit map_view(QWidget* parent);
}; // map_view

#endif // MAP_VIEW_H
