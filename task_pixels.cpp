// Andrew Naplavkov

#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_pixels.h"
#include "utilities.h"

void task_pixels::run(progress*)
{
  emit signal_scale(m_lr->snap_to_pixels(m_fr), m_lr->get_pj());
}
