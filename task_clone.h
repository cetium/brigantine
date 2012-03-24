// Andrew Naplavkov

#ifndef TASK_CLONE_H
#define TASK_CLONE_H

#include <QString>
#include <string>
#include <vector>
#include "connection_link.h"
#include "layer_link.h"
#include "task.h"

class task_clone : public task {
  Q_OBJECT
  layer_link m_lr_from;
  connection_link m_dbc_to;
  std::string m_tbl;
  bool m_sql;

signals:
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
  void signal_refresh(connection_link dbc);

public:
  task_clone(layer_link lr_from, connection_link dbc_to, const std::string& tbl, bool sql);
  virtual void run(progress* prg);
}; // task_clone

#endif // TASK_CLONE_H
