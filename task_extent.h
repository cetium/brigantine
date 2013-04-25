// Andrew Naplavkov

#ifndef TASK_EXTENT_H
#define TASK_EXTENT_H

#include <QRectF>
#include <QString>
#include "layer_ptr.h"
#include "projection.h"
#include "task.h"

class task_extent : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_rect(QRectF rect, projection pj);
public:
  explicit task_extent(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void run_impl() override;
}; // task_extent

#endif // TASK_EXTENT_H
