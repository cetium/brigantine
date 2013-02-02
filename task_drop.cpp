// Andrew Naplavkov

#include <iterator>
#include "layer.h"
#include "progress.h"
#include "provider.h"
#include "task_drop.h"

task_drop::task_drop(layer_ptr lr)
  : m_lr(lr)
{}

void task_drop::run(progress*)
{
  using namespace std;
  m_lr->unreg();
  auto pvd(m_lr->get_provider());
  for (size_t level(0), levels(m_lr->get_levels()); level < levels; ++level)
    pvd->drop(m_lr->get_table_def(level));
  emit signal_refresh(pvd);
}
