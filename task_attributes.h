// Andrew Naplavkov

#ifndef TASK_ATTRIBUTES_H
#define TASK_ATTRIBUTES_H

#include <memory>
#include <QString>
#include "layer_ptr.h"
#include "rowset_model.h"
#include "task.h"

class task_attributes : public task {
  Q_OBJECT
  layer_ptr m_lr;
signals:
  void signal_rowset(std::shared_ptr<rowset_model> rs);
public:
  explicit task_attributes(layer_ptr lr) : m_lr(lr)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run(QEventLoop& loop) override;
}; // task_attributes

#endif // TASK_ATTRIBUTES_H
