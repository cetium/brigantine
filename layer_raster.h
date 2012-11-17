// Andrew Naplavkov

#ifndef LAYER_RASTER_H
#define LAYER_RASTER_H

#include "layer.h"

class layer_raster : public layer {
  const brig::database::raster_pyramid m_raster;

  size_t get_level(const frame& fr);
  std::string get_raster_column(size_t level) const;

public:
  layer_raster(connection_link dbc, const brig::database::raster_pyramid& raster) : layer(dbc), m_raster(raster)  {}
  QString get_icon() override  { return ":/res/palette.png"; }

  brig::database::identifier get_identifier() override  { return m_raster.id; }
  size_t get_levels() override  { return m_raster.levels.size(); }
  brig::database::identifier get_geometry(size_t lvl) override  { return m_raster.levels[lvl].geometry; }
  brig::database::table_definition get_table_definition(size_t lvl) override;
  void reset_table_definitions() override;
  bool is_writable() override;

  layer* reg(connection_link dbc, std::vector<std::string>& sql) override;
  void unreg(std::vector<std::string>& sql) override;

  size_t limit() override  { return 100; }
  bool has_spatial_index(const frame& fr) override;
  std::shared_ptr<brig::database::rowset> attributes(const frame& fr) override;
  std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited) override;
  void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter) override;
}; // layer_raster

#endif // LAYER_RASTER_H
