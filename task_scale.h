// Andrew Naplavkov

#ifndef TASK_SCALE_H
#define TASK_SCALE_H

#include <brig/proj/shared_pj.hpp>
#include <QRectF>
#include "layer_ptr.h"
#include "task.h"

class task_scale : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_scale(double scale, brig::proj::shared_pj pj);
public:
  task_scale(layer_ptr lr) : m_lr(lr)  {}
  void run(progress* prg) override;
}; // task_scale

#endif // TASK_SCALE_H
