// Andrew Naplavkov

#include <iterator>
#include "layer.h"
#include "provider.h"
#include "task_drop.h"

QString task_drop::get_string()
{
  return QString("dropping '%1'").arg(m_lr->get_string(true));
}

void task_drop::run_impl()
{
  using namespace std;
  m_lr->unreg();
  auto pvd(m_lr->get_provider());
  for (size_t level(0), levels(m_lr->get_levels()); level < levels; ++level)
    pvd->drop(m_lr->get_table_def(level));
  emit signal_refresh(pvd);
}
