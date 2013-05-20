// Andrew Naplavkov

#include <algorithm>
#include <iterator>
#include "layer.h"
#include "layer_geometry.h"
#include "layer_raster.h"
#include "provider.h"
#include "task_connect.h"

QString task_connect::get_string()
{
  return QString("connecting to '%1'").arg(m_allocator->get_string());
}

void task_connect::run_impl()
{
  auto pvd(m_allocator->allocate());
  std::vector<layer_ptr> lrs;
  for (const auto& lr: pvd->get_geometry_layers()) lrs.emplace_back(new layer_geometry(pvd, lr));
  for (const auto& lr: pvd->get_raster_layers()) lrs.emplace_back(new layer_raster(pvd, lr));
  sort(begin(lrs), end(lrs), [](const layer_ptr& a, const layer_ptr& b){ return a->get_string() < b->get_string(); });
  pvd->reset_table_def();
  emit signal_connected(pvd, lrs);
  progress( QString("layers: %1").arg(lrs.size()) );
}
