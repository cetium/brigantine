// Andrew Naplavkov

#ifndef TASK_SCALE_H
#define TASK_SCALE_H

#include <brig/proj/shared_pj.hpp>
#include <QString>
#include "layer_ptr.h"
#include "task.h"

class task_scale : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_scale(double scale, brig::proj::shared_pj pj);
public:
  explicit task_scale(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run(QEventLoop& loop) override;
}; // task_scale

#endif // TASK_SCALE_H
