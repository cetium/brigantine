// Andrew Naplavkov

#ifndef TASK_INSERT_H
#define TASK_INSERT_H

#include <vector>
#include "insert_item.h"
#include "layer_link.h"
#include "task.h"

class task_insert : public task {
  Q_OBJECT
  layer_link m_lr_from, m_lr_to;
  std::vector<insert_item> m_items;
public:
  task_insert(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& items) : m_lr_from(lr_from), m_lr_to(lr_to), m_items(items)  {}
  static void run(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& items, progress* prg);
  virtual void run(progress* prg)  { run(m_lr_from, m_lr_to, m_items, prg); }
}; // task_insert

#endif // TASK_INSERT_H
