// Andrew Naplavkov

#ifndef TASK_DROP_H
#define TASK_DROP_H

#include <string>
#include <vector>
#include "layer_ptr.h"
#include "provider_ptr.h"
#include "task.h"

class task_drop : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_refresh(provider_ptr pvd);
public:
  explicit task_drop(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 2; }
  void run_impl() override;
}; // task_drop

#endif // TASK_DROP_H
