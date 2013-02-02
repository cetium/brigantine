// Andrew Naplavkov

#include "layer.h"
#include "progress.h"
#include "provider.h"
#include "task_mbr.h"
#include "utilities.h"

void task_mbr::run(progress* prg)
{
  brig::boost::box box;
  brig::proj::shared_pj pj;
  if (!m_lr->try_view(box, pj))
  {
    auto pvd(m_lr->get_provider());
    auto id(m_lr->get_geometry(0));
    auto tbl(pvd->get_table_def(id));
    pj = ::get_pj(*tbl[id.qualifier]);
    box = pvd->get_mbr(tbl, id.qualifier);
    pvd->set_mbr(id, box);
    if (!prg->step()) return;
  }
  emit signal_rect(box_to_rect(box), pj);
}
