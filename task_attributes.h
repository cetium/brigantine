// Andrew Naplavkov

#ifndef TASK_ATTRIBUTES_H
#define TASK_ATTRIBUTES_H

#include <string>
#include <vector>
#include "connection_link.h"
#include "frame.h"
#include "layer_link.h"
#include "task.h"

class task_attributes : public task {
  Q_OBJECT
  frame m_fr;
  layer_link m_lr;
signals:
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
public:
  task_attributes(frame fr, layer_link lr);
  virtual void run(progress* prg);
}; // task_attributes

#endif // TASK_ATTRIBUTES_H
