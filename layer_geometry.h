// Andrew Naplavkov

#ifndef LAYER_GEOMETRY_H
#define LAYER_GEOMETRY_H

#include "layer.h"

class layer_geometry : public layer {
  const brig::database::identifier m_id;
  const brig::database::table_definition m_tbl;

public:
  layer_geometry(connection_link dbc, const brig::database::identifier& id, const brig::database::table_definition& tbl = brig::database::table_definition());
  virtual QString get_icon()  { return ":/res/compasses.png"; }

  virtual brig::database::identifier get_identifier()  { return m_id; }
  virtual brig::database::identifier get_geometry()  { return m_id; }
  virtual size_t get_levels()  { return 1; }
  virtual brig::database::table_definition get_table_definition(size_t level);
  virtual void reset_table_definitions();

  virtual bool is_writable()  {return true; }
  virtual layer* create_result(connection_link dbc, const std::string& name, std::vector<std::string>& sql);
  virtual void drop_meta(std::vector<std::string>&)  {}

  virtual size_t limit()  { return 1000; }
  virtual std::shared_ptr<brig::database::rowset> attributes(const frame& fr);
  virtual std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited);
  virtual void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter);
}; // layer_geometry

#endif // LAYER_GEOMETRY_H
