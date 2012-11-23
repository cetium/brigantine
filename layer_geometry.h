// Andrew Naplavkov

#ifndef LAYER_GEOMETRY_H
#define LAYER_GEOMETRY_H

#include "layer.h"

class layer_geometry : public layer {
  const brig::database::identifier m_id;
  const brig::database::table_definition m_tbl;

public:
  layer_geometry(connection_link dbc, const brig::database::identifier& id, const brig::database::table_definition& tbl = brig::database::table_definition());
  QString get_icon() override  { return ":/res/compasses.png"; }

  brig::database::identifier get_identifier() override  { return m_id; }
  size_t get_levels() override  { return 1; }
  brig::database::identifier get_geometry(size_t) override  { return m_id; }
  brig::database::table_definition get_table_definition(size_t lvl) override;
  void reset_table_definitions() override;
  bool is_writable() override  { return true; }

  layer* reg(connection_link dbc, std::vector<std::string>& sql) override;
  void unreg(std::vector<std::string>&) override  {}

  size_t limit() override  { return 4 * brig::database::PageSize; }
  bool has_spatial_index(const frame& fr) override;
  std::shared_ptr<brig::database::rowset> attributes(const frame& fr) override;
  std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited) override;
  void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter) override;
}; // layer_geometry

#endif // LAYER_GEOMETRY_H
