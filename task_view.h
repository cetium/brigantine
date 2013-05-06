// Andrew Naplavkov

#ifndef TASK_VIEW_H
#define TASK_VIEW_H

#include <memory>
#include <QAction>
#include <QModelIndex>
#include <QPoint>
#include <QObject>
#include <QTableView>
#include "task.h"
#include "task_model.h"

class task_view : public QTableView {
  Q_OBJECT
  task_model m_mdl;
  QModelIndex m_idx_menu;
  QAction
    *m_cancel_act,
    *m_cancel_all_act,
    *m_clear_act;
private slots:
  void emit_progress();
  void emit_idle();
  void on_need_vacuum();
  void on_show_menu(QPoint point);
  void on_cancel();
  void on_cancel_all();
  void on_clear();
  void on_finished();
public slots:
  void on_task(std::shared_ptr<task> tsk);
signals:
  void signal_progress();
  void signal_idle();
public:
  explicit task_view(QWidget* parent);
}; // task_view

#endif // TASK_VIEW_H
