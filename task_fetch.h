// Andrew Naplavkov

#ifndef TASK_FETCH_H
#define TASK_FETCH_H

#include <memory>
#include <QString>
#include <string>
#include "provider_ptr.h"
#include "rowset_model.h"
#include "task.h"

class task_fetch : public task {
  Q_OBJECT
  provider_ptr m_pvd;
  std::string m_sql;
signals:
  void signal_rowset(std::shared_ptr<rowset_model> rs);
public:
  task_fetch(provider_ptr pvd, const std::string& sql) : m_pvd(pvd), m_sql(sql)  {}
  QString get_string() override;
  int get_priority() override  { return 1; }
  void do_run(QEventLoop& loop) override;
}; // task_fetch

#endif // TASK_FETCH_H
