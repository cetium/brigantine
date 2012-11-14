// Andrew Naplavkov

#ifndef SQL_VIEW_H
#define SQL_VIEW_H

#include <memory>
#include <QAction>
#include <QLabel>
#include <QTextEdit>
#include <QWidget>
#include "connection_link.h"
#include "sql_model.h"
#include "sql_thread.h"

class sql_view : public QWidget {
  Q_OBJECT

  std::shared_ptr<sql_model> m_mdl;
  sql_thread m_trd;
  connection_link m_dbc;
  QLabel* m_title;
  QAction *m_open, *m_save, *m_fetch, *m_run, *m_cancel, *m_info;
  QTextEdit *m_sql;

private slots:
  void emit_process(QString msg)  { emit signal_process(msg); }
  void on_cancel()  { m_trd.cancel(); }
  void on_fetch();
  void on_idle();
  void on_info();
  void on_open();
  void on_run();
  void on_save();
  void on_start();

public slots:
  void on_push(std::shared_ptr<task> tsk)  { m_trd.push(tsk); }
  void on_commands(connection_link dbc, std::vector<std::string> sqls);
  void on_disconnect(connection_link dbc);

signals:
  void signal_start();
  void signal_process(QString msg);
  void signal_idle();
  void signal_commands();

public:
  explicit sql_view(QWidget* parent);
}; // sql_view

#endif // SQL_VIEW_H
