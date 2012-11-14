// Andrew Naplavkov

#ifndef TASK_EXEC_H
#define TASK_EXEC_H

#include <string>
#include "connection_link.h"
#include "task.h"

class task_exec : public task {
  Q_OBJECT
  connection_link m_dbc;
  std::string m_sql;
signals:
  void signal_refresh(connection_link dbc);
public:
  task_exec(connection_link dbc, const std::string& sql);
  virtual void run(progress* prg);
}; // task_exec

#endif // TASK_EXEC_H
