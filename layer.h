// Andrew Naplavkov

#ifndef LAYER_H
#define LAYER_H

#include <brig/proj/epsg.hpp>
#include <memory>
#include <QPainter>
#include <QString>
#include <Qt>
#include <string>
#include <vector>
#include "connection.h"
#include "connection_link.h"
#include "frame.h"

class layer {
  connection_link m_dbc;

public:
  layer()  {}
  explicit layer(connection_link dbc) : m_dbc(dbc)  {}
  virtual ~layer()  {}
  virtual connection_link get_connection()  { return m_dbc; }
  virtual QString get_string() = 0;
  virtual QString get_icon() = 0;

  virtual size_t get_levels() = 0;
  virtual brig::database::identifier get_geometry_column(size_t level) = 0;
  virtual brig::database::table_definition get_table_definition(size_t level) = 0;

  virtual size_t limit() = 0;
  virtual std::shared_ptr<brig::database::rowset> attributes(const frame& fr) = 0;
  virtual std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited) = 0;
  virtual void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter) = 0;

  brig::proj::epsg get_epsg();
  bool get_mbr(brig::boost::box& box);
  brig::boost::box prepare_box(const frame& fr);
}; // layer

#endif // LAYER_H
