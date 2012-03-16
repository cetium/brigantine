// Andrew Naplavkov

#ifndef LAYER_GEOMETRY_H
#define LAYER_GEOMETRY_H

#include "layer.h"

class layer_geometry : public layer {
  brig::database::identifier m_id;
  brig::database::table_definition m_tbl;

public:
  layer_geometry(connection_link dbc, const brig::database::identifier& id) : layer(dbc), m_id(id)  {}
  layer_geometry(connection_link dbc, const brig::database::identifier& id, const brig::database::table_definition& tbl) : layer(dbc), m_id(id), m_tbl(tbl)  {}
  virtual QString get_string();
  virtual QString get_icon()  { return ":/compasses.png"; }

  virtual size_t get_levels()  { return 1; }
  virtual brig::database::identifier get_geometry_column(size_t)  { return m_id; }
  virtual brig::database::table_definition get_table_definition(size_t);

  virtual size_t limit()  { return 1000; }
  virtual std::shared_ptr<brig::database::rowset> attributes(const frame& fr);
  virtual std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited);
  virtual void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter);
}; // layer_geometry

#endif // LAYER_GEOMETRY_H
