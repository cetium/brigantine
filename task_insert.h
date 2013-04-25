// Andrew Naplavkov

#ifndef TASK_INSERT_H
#define TASK_INSERT_H

#include <vector>
#include "insert_item.h"
#include "layer_ptr.h"
#include "task.h"

class task_insert : public task {
  Q_OBJECT
  layer_ptr m_lr_from, m_lr_to;
  std::vector<insert_item> m_items;
  bool m_ccw, m_view;
  size_t m_counter;

public:
  task_insert(layer_ptr lr_from, layer_ptr lr_to, const std::vector<insert_item>& items, bool ccw, bool view, size_t counter = 0)
    : m_lr_from(lr_from), m_lr_to(lr_to), m_items(items), m_ccw(ccw), m_view(view), m_counter(counter)
    {}
  size_t get_counter() const  { return m_counter; }

  QString get_string() override;
  int get_priority() override  { return 3; }
  void run_impl() override;
}; // task_insert

#endif // TASK_INSERT_H
