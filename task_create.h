// Andrew Naplavkov

#ifndef TASK_CREATE_H
#define TASK_CREATE_H

#include <QString>
#include <string>
#include <vector>
#include "provider_ptr.h"
#include "layer_ptr.h"
#include "task.h"

class task_create : public task {
  Q_OBJECT
  std::vector<layer_ptr> m_lrs_from;
  provider_ptr m_pvd_to;
  bool m_sql, m_view;

signals:
  void signal_sql(provider_ptr pvd, std::vector<std::string> sqls);
  void signal_refresh(provider_ptr pvd);

public:
  task_create(std::vector<layer_ptr> lrs_from, provider_ptr pvd_to, bool sql, bool view);
  void run(progress* prg) override;
}; // task_create

#endif // TASK_CREATE_H
