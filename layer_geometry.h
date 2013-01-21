// Andrew Naplavkov

#ifndef LAYER_GEOMETRY_H
#define LAYER_GEOMETRY_H

#include "layer.h"

class layer_geometry : public layer {
  const brig::identifier m_id;
  const brig::table_definition m_tbl;

  layer_geometry(connection_link dbc, const brig::table_definition& tbl);

public:
  layer_geometry(connection_link dbc, const brig::identifier& id);
  QString get_icon() override  { return ":/res/compasses.png"; }

  brig::identifier get_identifier() override  { return m_id; }
  size_t get_levels() override  { return 1; }
  brig::identifier get_geometry(size_t) override  { return m_id; }
  brig::table_definition get_table_definition(size_t lvl) override;
  void reset_table_definitions() override;
  bool is_raster() override  { return false; }

  layer* fit(connection_link dbc) override;
  void reg() override  {}
  void reg(std::vector<std::string>&) override  {}
  void unreg() override  {}

  std::shared_ptr<brig::rowset> attributes(const frame& fr) override;
  std::shared_ptr<brig::rowset> drawing(const frame& fr) override;
  void draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter) override;
  bool has_spatial_index(const frame& fr) override;
  frame snap_to_pixels(const frame& fr) override  { return fr; }
}; // layer_geometry

#endif // LAYER_GEOMETRY_H
