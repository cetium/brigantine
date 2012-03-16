// Andrew Naplavkov

#include "connection.h"
#include "progress.h"
#include "task_exec.h"

task_exec::task_exec(connection_link dbc, const std::vector<std::string>& sqls) : m_dbc(dbc), m_sqls(sqls)  {}

void task_exec::run(progress* prg)
{
  auto cmd(m_dbc->get_command());
  for (auto sql(std::begin(m_sqls)); sql != std::end(m_sqls); ++sql)
  {
    cmd->exec(*sql);
    if (!prg->step(cmd->affected())) return;
  }
  emit signal_refresh(m_dbc);
}
