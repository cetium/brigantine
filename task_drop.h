// Andrew Naplavkov

#ifndef TASK_DROP_H
#define TASK_DROP_H

#include <string>
#include <vector>
#include "connection_link.h"
#include "layer_link.h"
#include "task.h"

class task_drop : public task {
  Q_OBJECT
  layer_link m_lr;
signals:
  void signal_refresh(connection_link dbc);
public:
  task_drop(layer_link lr);
  void run(progress* prg) override;
}; // task_drop

#endif // TASK_DROP_H
