// Andrew Naplavkov

#include <boost/geometry/geometry.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
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
  if (col.query_value.type() != typeid(brig::blob_t)) return false;
  const brig::blob_t& blob(boost::get<brig::blob_t>(col.query_value));
  if (blob.empty()) return false;
  box = brig::boost::envelope(brig::boost::geom_from_wkb(blob));
  return true;
}

brig::database::variant layer::prepare_box(const frame& fr)
{
  brig::boost::box mbr, box;
  box = rect_to_box(transform(fr.prepare_rect(), fr.get_epsg(), get_epsg()));
  if (get_mbr(mbr) && boost::geometry::covered_by(mbr, box)) return brig::database::null_t();
  if (get_epsg().is_latlong()) // "out of longitude/latitude" workaround
  {
    const brig::boost::box tmp(box);
    const brig::boost::box wrld(brig::boost::point(-180, -90), brig::boost::point(180, 90));
    boost::geometry::intersection(tmp, wrld, box);
  }
  return brig::boost::as_binary(box);
}
