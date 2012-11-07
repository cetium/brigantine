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
    auto tbl(dbc->get_table_definition(id));
    pj = brig::proj::epsg(tbl[id.qualifier]->epsg);
    box = dbc->get_mbr(tbl, id.qualifier);
    dbc->set_mbr(id, box);
    if (!prg->step()) return;
  }
  emit signal_view(box_to_rect(box), pj);
}
