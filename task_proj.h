// Andrew Naplavkov

#ifndef TASK_PROJ_H
#define TASK_PROJ_H

#include <QString>
#include "layer_ptr.h"
#include "projection.h"
#include "task.h"

class task_proj : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_proj(projection pj);
public:
  explicit task_proj(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run(QEventLoop& loop) override;
}; // task_proj

#endif // TASK_PROJ_H
