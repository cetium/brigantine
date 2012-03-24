// Andrew Naplavkov

#ifndef TASK_INSERT_H
#define TASK_INSERT_H

#include "insert_map.h"
#include "layer_link.h"
#include "task.h"

class task_insert : public task {
  Q_OBJECT
  layer_link m_lr_from, m_lr_to;
  insert_map m_map;
public:
  task_insert(layer_link lr_from, layer_link lr_to, const insert_map& map) : m_lr_from(lr_from), m_lr_to(lr_to), m_map(map)  {}
  static void run(layer_link lr_from, layer_link lr_to, const insert_map& map, progress* prg);
  virtual void run(progress* prg)  { run(m_lr_from, m_lr_to, m_map, prg); }
}; // task_insert

#endif // TASK_INSERT_H
