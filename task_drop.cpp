// Andrew Naplavkov

#include <iterator>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_drop.h"

task_drop::task_drop(layer_link lr)
  : m_lr(lr)
{}

void task_drop::run(progress*)
{
  using namespace std;
  m_lr->unreg();
  auto dbc(m_lr->get_connection());
  for (size_t level(0), levels(m_lr->get_levels()); level < levels; ++level)
    dbc->drop(m_lr->get_table_def(level));
  emit signal_refresh(dbc);
}
