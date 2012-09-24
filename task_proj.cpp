// Andrew Naplavkov

#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_proj.h"

void task_proj::run(progress* prg)
{
  brig::proj::epsg pj(m_lr->get_epsg());
  if (!prg->step()) return;
  emit signal_proj(pj);
}
