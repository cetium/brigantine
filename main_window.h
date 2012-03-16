// Andrew Naplavkov

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <brig/proj/epsg.hpp>
#include <QAction>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTabWidget>
#include <QTime>
#include <QWidget>

class main_window : public QMainWindow {
  Q_OBJECT

  QTabWidget* m_tab;
  int m_map_tab, m_sql_tab;
  QLabel *m_pos_stat, *m_map_stat, *m_sql_stat;
  QString m_map_msg, m_sql_msg;
  QTime m_map_time, m_sql_time;
  QAction *m_copy_map_stat, *m_copy_sql_stat;

protected:
  virtual void keyPressEvent(QKeyEvent* event);

private slots:
  void on_map_coords(const QString& msg);
  void on_map_start();
  void on_map_process(const QString& msg)  { m_map_msg = msg; }
  void on_map_idle();
  void on_map_scene(brig::proj::epsg pj);
  void on_sql_start();
  void on_sql_process(const QString& msg)  { m_sql_msg = msg; }
  void on_sql_idle();
  void on_sql_commands()  { m_tab->setCurrentIndex(m_sql_tab); }
  void on_timer();
  void show_stat_menu(const QPoint&);
  void copy_map_stat();
  void copy_sql_stat();

public:
  explicit main_window(QWidget* parent = 0);
}; // main_window

#endif // MAIN_WINDOW_H
