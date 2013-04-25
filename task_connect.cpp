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
  using namespace std;
  size_t counter(0);
  auto pvd(m_allocator->allocate());
  vector<layer_ptr> lrs;
  auto geometries(pvd->get_geometry_layers());
  for (auto lr(begin(geometries)); lr != end(geometries); ++lr, ++counter)
    lrs.emplace_back(new layer_geometry(pvd, *lr));
  auto rasters(pvd->get_raster_layers());
  for (auto lr(begin(rasters)); lr != end(rasters); ++lr, ++counter)
    lrs.emplace_back(new layer_raster(pvd, *lr));
  sort(begin(lrs), end(lrs), [](const layer_ptr& a, const layer_ptr& b){ return a->get_string() < b->get_string(); });
  pvd->reset_table_def();
  emit signal_connected(pvd, lrs);
  progress( QString("layers: %1").arg(counter) );
}
