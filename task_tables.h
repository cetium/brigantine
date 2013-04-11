// Andrew Naplavkov

#ifndef TASK_TABLES_H
#define TASK_TABLES_H

#include <memory>
#include <QString>
#include "provider_ptr.h"
#include "rowset_model.h"
#include "task.h"

class task_tables : public task {
  Q_OBJECT
  provider_ptr m_pvd;
signals:
  void signal_rowset(std::shared_ptr<rowset_model> rs);
public:
  task_tables(provider_ptr pvd) : m_pvd(pvd)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run(QEventLoop& loop) override;
}; // task_tables

#endif // TASK_TABLES_H
