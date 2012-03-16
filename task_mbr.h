// Andrew Naplavkov

#ifndef TASK_MBR_H
#define TASK_MBR_H

#include <brig/proj/epsg.hpp>
#include <QRectF>
#include "layer_link.h"
#include "task.h"

class task_mbr : public task {
  Q_OBJECT
  layer_link m_lr;
signals:
  void signal_view(const QRectF& rect, const brig::proj::epsg& pj);
public:
  task_mbr(layer_link lr) : m_lr(lr)  {}
  virtual void run(progress* prg);
}; // task_mbr

#endif // TASK_MBR_H
