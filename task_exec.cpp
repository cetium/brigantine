// Andrew Naplavkov

#include "connection.h"
#include "task_exec.h"

task_exec::task_exec(connection_link dbc, const std::string& sql) : m_dbc(dbc), m_sql(sql)
{
}

void task_exec::run(progress*)
{
  auto cmd(m_dbc->get_command());
  cmd->exec_batch(m_sql);
  emit signal_refresh(m_dbc);
}
