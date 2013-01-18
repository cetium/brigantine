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
  bool m_ccw, m_view;

public:
  size_t m_counter;

  task_insert(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& items, bool ccw, bool view)
    : m_lr_from(lr_from), m_lr_to(lr_to), m_items(items), m_ccw(ccw), m_view(view), m_counter(0)
    {}
  void run(progress* prg) override;
}; // task_insert

#endif // TASK_INSERT_H
