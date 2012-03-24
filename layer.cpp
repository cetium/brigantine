// Andrew Naplavkov

#include "layer.h"
#include "utilities.h"

QString layer::get_string()
{
  QString str;
  auto id(get_identifier());
  if (!id.schema.empty()) str += QString::fromUtf8(id.schema.c_str()) + ".";
  str += QString::fromUtf8(id.name.c_str()) + "." + QString::fromUtf8(id.qualifier.c_str());
  return str;
}

brig::proj::epsg layer::get_epsg()
{
  return ::get_epsg(get_connection()->get_column_definition(get_geometry()).epsg);
}

bool layer::get_mbr(brig::boost::box& box)
{
  auto col(get_connection()->get_column_definition(get_geometry()));
  if (col.mbr.type() != typeid(brig::boost::box)) return false;
  box = boost::get<brig::boost::box>(col.mbr);
  return true;
}

brig::boost::box layer::prepare_box(const frame& fr)
{
  return rect_to_box(transform(fr.prepare_rect(), fr.get_epsg(), get_epsg()));
}
