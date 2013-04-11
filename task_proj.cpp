// Andrew Naplavkov

#include "layer.h"
#include "task_proj.h"

QString task_proj::get_string()
{
  return QString("proj of '%1'").arg(m_lr->get_string(true));
}

void task_proj::do_run(QEventLoop&)
{
  emit signal_proj(m_lr->get_pj());
}
