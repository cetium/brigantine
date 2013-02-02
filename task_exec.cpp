// Andrew Naplavkov

#include "provider.h"
#include "task_exec.h"

task_exec::task_exec(provider_ptr pvd, const std::string& sql)
  : m_pvd(pvd), m_sql(sql)
{}

void task_exec::run(progress*)
{
  auto cmd(m_pvd->get_command());
  cmd->exec_batch(m_sql);
  emit signal_refresh(m_pvd);
}
