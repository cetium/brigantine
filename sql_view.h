// Andrew Naplavkov

#ifndef SQL_VIEW_H
#define SQL_VIEW_H

#include <memory>
#include <QAction>
#include <QLabel>
#include <QTableView>
#include <QTextEdit>
#include <QWidget>
#include "provider_ptr.h"
#include "rowset_model.h"
#include "task.h"

class sql_view : public QWidget {
  Q_OBJECT

  std::shared_ptr<rowset_model> m_mdl;
  provider_ptr m_pvd;
  QLabel* m_title;
  QAction *m_open, *m_save, *m_run, *m_info;
  QTextEdit *m_sql;
  QTableView *m_rowset;
  size_t m_tasks;

  void reset();

private slots:
  void on_finished(QString);
  void on_info();
  void on_open();
  void on_run();
  void on_save();


public slots:
  void on_sql(provider_ptr pvd, std::vector<std::string> sqls);
  void on_disconnect(provider_ptr pvd);
  void on_rowset(std::shared_ptr<rowset_model> rs);

signals:
  void signal_sql();
  void signal_task(std::shared_ptr<task> tsk);
  void signal_progress();
  void signal_idle();

public:
  explicit sql_view(QWidget* parent);
}; // sql_view

#endif // SQL_VIEW_H
