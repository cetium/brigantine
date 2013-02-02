// Andrew Naplavkov

#include "layer.h"
#include "progress.h"
#include "task_proj.h"

void task_proj::run(progress* prg)
{
  brig::proj::shared_pj pj(m_lr->get_pj());
  if (!prg->step()) return;
  emit signal_proj(pj);
}
