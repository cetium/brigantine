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
  brig::database::variant prepare_box(const frame& fr);

public:
  layer()  {}
  explicit layer(connection_link dbc) : m_dbc(dbc)  {}
  virtual ~layer()  {}
  virtual QString get_icon() = 0;

  virtual brig::database::identifier get_identifier() = 0;
  virtual size_t get_levels() = 0;
  virtual brig::database::identifier get_geometry(size_t lvl) = 0;
  virtual brig::database::table_definition get_table_definition(size_t lvl) = 0;
  virtual void reset_table_definitions() = 0;
  virtual bool is_writable() = 0;

  virtual layer* reg(connection_link dbc, std::vector<std::string>& sql) = 0;
  virtual void unreg(std::vector<std::string>& sql) = 0;

  virtual size_t limit() = 0;
  virtual bool has_spatial_index(const frame& fr) = 0;
  virtual std::shared_ptr<brig::database::rowset> attributes(const frame& fr) = 0;
  virtual std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited) = 0;
  virtual void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter) = 0;

  connection_link get_connection()  { return m_dbc; }
  QString get_string();
  brig::proj::shared_pj get_pj();
  bool try_pj(brig::proj::shared_pj& pj);
  bool try_view(brig::boost::box& box, brig::proj::shared_pj& pj);
}; // layer

#endif // LAYER_H
