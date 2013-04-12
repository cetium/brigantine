// Andrew Naplavkov

#include "layer.h"
#include "provider.h"
#include "task_extent.h"
#include "utilities.h"

QString task_extent::get_string()
{
  return QString("extent of '%1'").arg(m_lr->get_string(true));
}

void task_extent::do_run(QEventLoop&)
{
  using namespace std;
  brig::boost::box box;
  projection pj;
  if (!m_lr->try_view(box, pj))
  {
    auto pvd(m_lr->get_provider());
    auto id(m_lr->get_geometry(0));
    auto tbl(pvd->get_table_def(id));
    pj = ::get_pj(*tbl[id.qualifier]);
    tbl.query_columns.push_back(id.qualifier);
    box = pvd->get_extent(tbl);
    pvd->set_extent(id, box);
  }
  emit signal_rect(box_to_rect(box), pj);
}
