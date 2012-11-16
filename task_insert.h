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
  bool m_ccw;
public:
  task_insert(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& items, bool ccw)
    : m_lr_from(lr_from), m_lr_to(lr_to), m_items(items), m_ccw(ccw)
    {}
  static void run(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& items, bool ccw, size_t& counter, progress* prg);
  void run(progress* prg) override;
}; // task_insert

#endif // TASK_INSERT_H
