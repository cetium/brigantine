// Andrew Naplavkov

#include "layer.h"
#include "progress.h"
#include "provider.h"
#include "task_extent.h"
#include "utilities.h"

void task_extent::run(progress* prg)
{
  brig::boost::box box;
  brig::proj::shared_pj pj;
  if (!m_lr->try_view(box, pj))
  {
    auto pvd(m_lr->get_provider());
    auto id(m_lr->get_geometry(0));
    auto tbl(pvd->get_table_def(id));
    pj = ::get_pj(*tbl[id.qualifier]);
    tbl.query_columns.push_back(id.qualifier);
    box = pvd->get_extent(tbl);
    pvd->set_extent(id, box);
    if (!prg->step()) return;
  }
  emit signal_rect(box_to_rect(box), pj);
}
