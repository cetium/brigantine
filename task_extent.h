// Andrew Naplavkov

#ifndef TASK_EXTENT_H
#define TASK_EXTENT_H

#include <brig/proj/shared_pj.hpp>
#include <QRectF>
#include "layer_ptr.h"
#include "task.h"

class task_extent : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_rect(QRectF rect, brig::proj::shared_pj pj);
public:
  explicit task_extent(layer_ptr lr) : m_lr(lr)  {}
  void run(progress* prg) override;
}; // task_extent

#endif // TASK_EXTENT_H
