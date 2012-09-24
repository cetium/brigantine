// Andrew Naplavkov

#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_mbr.h"
#include "utilities.h"

void task_mbr::run(progress* prg)
{
  brig::boost::box box;
  brig::proj::epsg pj;
  if (!m_lr->try_view(box, pj))
  {
    auto dbc(m_lr->get_connection());
    auto id(m_lr->get_geometry());
    auto col(dbc->get_column_definition(id));
    pj = brig::proj::epsg(col.epsg);
    box = dbc->get_mbr(id, col);
    dbc->set_mbr(id, box);
    if (!prg->step()) return;
  }
  emit signal_view(box_to_rect(box), pj);
}
