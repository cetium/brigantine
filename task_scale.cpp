// Andrew Naplavkov

#include "layer.h"
#include "progress.h"
#include "task_scale.h"
#include "utilities.h"

void task_scale::run(progress*)
{
  emit signal_scale(m_lr->native_scale(m_fr), m_lr->get_pj());
}
