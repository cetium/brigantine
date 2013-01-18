// Andrew Naplavkov

#ifndef TASK_ATTRIBUTES_H
#define TASK_ATTRIBUTES_H

#include <string>
#include <vector>
#include "connection_link.h"
#include "layer_link.h"
#include "task.h"

class task_attributes : public task {
  Q_OBJECT
  layer_link m_lr;
signals:
  void signal_sql(connection_link dbc, std::vector<std::string> sqls);
public:
  task_attributes(layer_link lr) : m_lr(lr)  {}
  void run(progress* prg) override;
}; // task_attributes

#endif // TASK_ATTRIBUTES_H
