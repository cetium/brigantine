// Andrew Naplavkov

#ifndef TASK_PROJ_H
#define TASK_PROJ_H

#include <brig/proj/shared_pj.hpp>
#include <QRectF>
#include "layer_link.h"
#include "task.h"

class task_proj : public task {
  Q_OBJECT
  layer_link m_lr;
signals:
  void signal_proj(brig::proj::shared_pj pj);
public:
  task_proj(layer_link lr) : m_lr(lr)  {}
  virtual void run(progress* prg);
}; // task_proj

#endif // TASK_PROJ_H
