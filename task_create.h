// Andrew Naplavkov

#ifndef TASK_CREATE_H
#define TASK_CREATE_H

#include <QString>
#include <string>
#include <vector>
#include "connection_link.h"
#include "layer_link.h"
#include "task.h"

class task_create : public task {
  Q_OBJECT
  layer_link m_lr_from;
  connection_link m_dbc_to;
  std::string m_tbl;
  bool m_sql;

signals:
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
  void signal_refresh(connection_link dbc);

public:
  task_create(layer_link lr_from, connection_link dbc_to, const std::string& tbl, bool sql);
  virtual void run(progress* prg);
}; // task_create

#endif // TASK_CREATE_H