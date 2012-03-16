// Andrew Naplavkov

#include "layer.h"
#include "utilities.h"

brig::proj::epsg layer::get_epsg()
{
  return ::get_epsg(get_connection()->get_column_definition(get_geometry_column(0)).epsg);
}

bool layer::get_mbr(brig::boost::box& box)
{
  auto col(get_connection()->get_column_definition(get_geometry_column(0)));
  if (col.mbr.type() != typeid(brig::boost::box)) return false;
  box = boost::get<brig::boost::box>(col.mbr);
  return true;
}

brig::boost::box layer::prepare_box(const frame& fr)
{
  return rect_to_box(transform(fr.prepare_rect(), fr.get_epsg(), get_epsg()));
}
