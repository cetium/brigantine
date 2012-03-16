// Andrew Naplavkov

#ifndef TASK_EXEC_H
#define TASK_EXEC_H

#include <string>
#include <vector>
#include "connection_link.h"
#include "task.h"

class task_exec : public task {
  Q_OBJECT
  connection_link m_dbc;
  std::vector<std::string> m_sqls;
signals:
  void signal_refresh(connection_link dbc);
public:
  task_exec(connection_link dbc, const std::vector<std::string>& sqls);
  virtual void run(progress* prg);
}; // task_exec

#endif // TASK_EXEC_H
