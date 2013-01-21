// Andrew Naplavkov

#ifndef LAYER_H
#define LAYER_H

#include <brig/proj/shared_pj.hpp>
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

protected:
  brig::variant prepare_box(const frame& fr);

public:
  layer()  {}
  explicit layer(connection_link dbc) : m_dbc(dbc)  {}
  virtual ~layer()  {}
  virtual QString get_icon() = 0;

  virtual brig::identifier get_identifier() = 0;
  virtual size_t get_levels() = 0;
  virtual brig::identifier get_geometry(size_t lvl) = 0;
  virtual brig::table_definition get_table_definition(size_t lvl) = 0;
  virtual void reset_table_definitions() = 0;
  virtual bool is_raster() = 0;

  virtual layer* fit(connection_link dbc) = 0;
  virtual void reg() = 0;
  virtual void reg(std::vector<std::string>& sql) = 0;
  virtual void unreg() = 0;

  virtual std::shared_ptr<brig::rowset> attributes(const frame& fr) = 0;
  virtual std::shared_ptr<brig::rowset> drawing(const frame& fr) = 0;
  virtual void draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter) = 0;
  virtual bool has_spatial_index(const frame& fr) = 0;
  virtual frame snap_to_pixels(const frame& fr) = 0;

  connection_link get_connection()  { return m_dbc; }
  QString get_string();
  brig::proj::shared_pj get_pj();
  bool try_pj(brig::proj::shared_pj& pj);
  bool try_view(brig::boost::box& box, brig::proj::shared_pj& pj);
}; // layer

#endif // LAYER_H
