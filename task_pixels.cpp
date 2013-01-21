// Andrew Naplavkov

#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_pixels.h"
#include "utilities.h"

void task_pixels::run(progress* prg)
{
  const frame fr(m_lr->snap_to_pixels(m_fr));
  if (!prg->step()) return;
  emit signal_proj(fr.get_pj());
  emit signal_rect(fr.prepare_rect(), fr.get_pj());
}
