// Andrew Naplavkov

#ifndef TASK_ATTRIBUTES_H
#define TASK_ATTRIBUTES_H

#include <string>
#include <vector>
#include "layer_ptr.h"
#include "provider_ptr.h"
#include "task.h"

class task_attributes : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_sql(provider_ptr pvd, std::vector<std::string> sqls);
public:
  explicit task_attributes(layer_ptr lr) : m_lr(lr)  {}
  void run(progress* prg) override;
}; // task_attributes

#endif // TASK_ATTRIBUTES_H
