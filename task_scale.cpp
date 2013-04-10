// Andrew Naplavkov

#include "layer.h"
#include "task_scale.h"

QString task_scale::get_string()
{
  return QString("scale of '%1'").arg(m_lr->get_string(true));
}

void task_scale::do_run()
{
  emit signal_scale(m_lr->native_scale(m_fr), m_lr->get_pj());
}
