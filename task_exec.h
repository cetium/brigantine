// Andrew Naplavkov

#ifndef TASK_EXEC_H
#define TASK_EXEC_H

#include <string>
#include "provider_ptr.h"
#include "task.h"

class task_exec : public task {
  Q_OBJECT
  provider_ptr m_pvd;
  std::string m_sql;
signals:
  void signal_refresh(provider_ptr pvd);
public:
  task_exec(provider_ptr pvd, const std::string& sql);
  void run(progress* prg) override;
}; // task_exec

#endif // TASK_EXEC_H
