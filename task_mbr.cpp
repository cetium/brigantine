// Andrew Naplavkov

#include <stdexcept>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_mbr.h"
#include "utilities.h"

void task_mbr::run(progress* prg)
{
  auto dbc(m_lr->get_connection());
  auto id(m_lr->get_geometry());
  auto col(dbc->get_column_definition( id ));
  auto box(dbc->get_mbr(id, col));
  dbc->set_mbr(id, box);
  if (!prg->step()) return;
  emit signal_view(box_to_rect(box), m_lr->get_epsg());
}
