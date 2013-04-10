// Andrew Naplavkov

#ifndef TASK_PROJ_H
#define TASK_PROJ_H

#include <brig/proj/shared_pj.hpp>
#include <QString>
#include "layer_ptr.h"
#include "task.h"

class task_proj : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_proj(brig::proj::shared_pj pj);
public:
  explicit task_proj(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run() override;
}; // task_proj

#endif // TASK_PROJ_H
