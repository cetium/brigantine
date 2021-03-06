// Andrew Naplavkov

#include <boost/geometry/geometry.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <exception>
#include "layer.h"
#include "transformer.h"
#include "utilities.h"

QString layer::get_string(bool full)
{
  auto id(get_identifier());
  auto str = QString("%1.%2").arg(QString::fromUtf8(id.to_string().c_str())).arg(QString::fromUtf8(id.qualifier.c_str()));
  return full? QString("%1 (%2)").arg(str).arg(get_provider()->get_string()): str;
}

projection layer::get_pj()
{
  auto id(get_geometry(0));
  auto tbl(get_provider()->get_table_def(id));
  return ::get_pj(*tbl[id.qualifier]);
}

bool layer::try_pj(projection& pj)
{
  try
  {
    auto geo(get_geometry(0));
    brig::table_def tbl;
    if (!get_provider()->try_table_def(geo, tbl)) return false;
    pj = ::get_pj(*tbl[geo.qualifier]);
    return true;
  }
  catch (const std::exception&)  { return false; }
}

bool layer::try_view(brig::boost::box& box, projection& pj)
{
  try
  {
    auto geo(get_geometry(0));
    brig::table_def tbl;
    if (!get_provider()->try_table_def(geo, tbl)) return false;
    auto col(tbl[geo.qualifier]);
    pj = ::get_pj(*col);
    if (col->query_value.type() != typeid(brig::blob_t)) return false;
    const brig::blob_t& blob(boost::get<brig::blob_t>(col->query_value));
    if (blob.empty()) return false;
    box = brig::boost::envelope(brig::boost::geom_from_wkb(blob));
    return true;
  }
  catch (const std::exception&)  { return false; }
}

brig::variant layer::prepare_box(const frame& fr)
{
  brig::boost::box mbr, box;
  transformer tr(fr.get_pj(), get_pj());
  box = rect_to_box(tr.transform(fr.prepare_rect()));
  projection pj;
  if (try_view(mbr, pj) && boost::geometry::covered_by(mbr, box))
    return brig::null_t();
  if (tr.to_latlong()) // "out of longitude/latitude" workaround
  {
    const brig::boost::box tmp(box);
    const brig::boost::box wrld(brig::boost::point(-180, -90), brig::boost::point(180, 90));
    boost::geometry::intersection(tmp, wrld, box);
  }
  return brig::boost::as_binary(box);
}
