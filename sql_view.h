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
  QAction *m_info, *m_fetch, *m_run, *m_cancel;
  QTextEdit *m_sql;

private slots:
  void info();
  void fetch();
  void run();
  void cancel()  { m_trd.cancel(); }
  void on_start();
  void on_process(const QString& msg)  { emit signal_process(msg); }
  void on_idle();

public slots:
  void push(std::shared_ptr<task> tsk)  { m_trd.push(tsk); }
  void on_commands(connection_link dbc, std::vector<std::string> sqls);
  void on_disconnect(connection_link dbc);

signals:
  void signal_start();
  void signal_process(const QString& msg);
  void signal_idle();
  void signal_commands();

public:
  explicit sql_view(QWidget* parent);
}; // sql_view

#endif // SQL_VIEW_H
