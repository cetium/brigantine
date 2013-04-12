// Andrew Naplavkov

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include "clickable_label.h"
#include "projection.h"

class main_window : public QMainWindow {
  Q_OBJECT

  QTabWidget* m_tab;
  int m_map_tab, m_sql_tab;
  QLabel *m_proj_stat;
  clickable_label *m_pos_stat;
  QString m_proj_msg, m_pos_msg;
  QAction *m_copy_proj_stat;

private slots:
  void on_map_coords(QString msg);
  void on_map_proj(projection pj);
  void on_map_progress();
  void on_map_idle();
  void on_sql();
  void on_sql_progress();
  void on_sql_idle();
  void on_show_stat_menu(QPoint);
  void on_copy_proj_stat();

protected:
  void keyPressEvent(QKeyEvent* event) override;

public:
  main_window();
}; // main_window

#endif // MAIN_WINDOW_H
