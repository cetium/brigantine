// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_attributes.h"
#include "utilities.h"

task_attributes::task_attributes(frame fr, layer_link lr) : m_fr(fr), m_lr(lr)  {}

void task_attributes::run(progress* prg)
{
  auto rs(m_lr->attributes(m_fr));
  prg->init(rs->columns());
  std::vector<brig::database::variant> row;
  for (size_t counter(1); rs->fetch(row); ++counter)
  {
    std::vector<std::string> str_row;
    for (auto iter(std::begin(row)); iter != std::end(row); ++iter)
      str_row.push_back(brig::string_cast<char>(*iter));
    if (!prg->step(counter, str_row)) return;
  }
  emit signal_commands(m_lr->get_connection(), std::vector<std::string>());
}
