// Andrew Naplavkov

#include "layer.h"
#include "task_proj.h"

void task_proj::run(progress*)
{
  emit signal_proj(m_lr->get_pj());
}
