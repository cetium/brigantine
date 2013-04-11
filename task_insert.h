// Andrew Naplavkov

#ifndef TASK_INSERT_H
#define TASK_INSERT_H

#include <QEventLoop>
#include <QTime>
#include <vector>
#include "insert_item.h"
#include "layer_ptr.h"
#include "task.h"

class task_insert : public task {
  Q_OBJECT

  layer_ptr m_lr_from, m_lr_to;
  std::vector<insert_item> m_items;
  bool m_ccw, m_view;

public:
  task_insert(layer_ptr lr_from, layer_ptr lr_to, const std::vector<insert_item>& items, bool ccw, bool view)
    : m_lr_from(lr_from), m_lr_to(lr_to), m_items(items), m_ccw(ccw), m_view(view)
    {}
  QString get_string() override;
  int get_priority() override  { return 3; }
  void do_run(QEventLoop& loop) override;
  void do_run(QTime& time, size_t& counter, QEventLoop& loop, bool& cancel);
}; // task_insert

#endif // TASK_INSERT_H
