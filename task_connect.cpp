// Andrew Naplavkov

#include <algorithm>
#include <iterator>
#include "progress.h"
#include "layer.h"
#include "layer_geometry.h"
#include "layer_raster.h"
#include "provider.h"
#include "task_connect.h"

void task_connect::run(progress* prg)
{
  using namespace std;

  size_t counter(0);
  for (auto allocator(begin(m_allocators)); allocator != end(m_allocators); ++allocator)
  {
    auto pvd((*allocator)->allocate());
    vector<layer_ptr> lrs;

    auto geometries(pvd->get_geometry_layers());
    for (auto lr(begin(geometries)); lr != end(geometries); ++lr)
    {
      if (!prg->step(++counter)) return;
      lrs.emplace_back(new layer_geometry(pvd, *lr));
    }

    auto rasters(pvd->get_raster_layers());
    for (auto lr(begin(rasters)); lr != end(rasters); ++lr)
    {
      if (!prg->step(++counter)) return;
      lrs.emplace_back(new layer_raster(pvd, *lr));
    }

    sort(begin(lrs), end(lrs), [](const layer_ptr& a, const layer_ptr& b){ return a->get_string() < b->get_string(); });
    emit signal_connected(pvd, lrs);
  }
  prg->step(counter);
}
