// Andrew Naplavkov

#ifndef TASK_PIXELS_H
#define TASK_PIXELS_H

#include <brig/proj/shared_pj.hpp>
#include <QRectF>
#include "layer_link.h"
#include "task.h"

class task_pixels : public task {
  Q_OBJECT
  layer_link m_lr;
signals:
  void signal_scale(double scale, brig::proj::shared_pj pj);
public:
  task_pixels(layer_link lr) : m_lr(lr)  {}
  void run(progress* prg) override;
}; // task_pixels

#endif // TASK_PIXELS_H
