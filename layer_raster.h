// Andrew Naplavkov

#ifndef LAYER_RASTER_H
#define LAYER_RASTER_H

#include "layer.h"

class layer_raster : public layer {
  const brig::pyramid_def m_raster;
  std::vector<brig::table_def> m_tbls;

  size_t get_level(const frame& fr);
  std::string get_raster_column(size_t level) const;

  layer_raster(connection_link dbc, const brig::pyramid_def& raster, const std::vector<brig::table_def>& tbls);

public:
  layer_raster(connection_link dbc, const brig::pyramid_def& raster);
  QString get_icon() override  { return ":/res/palette.png"; }

  brig::identifier get_identifier() override  { return m_raster.id; }
  size_t get_levels() override  { return m_raster.levels.size(); }
  brig::identifier get_geometry(size_t lvl) override  { return m_raster.levels[lvl].geometry; }
  brig::table_def get_table_def(size_t lvl) override;
  void reset_table_defs() override;
  bool is_raster() override  { return true; }

  layer* fit(connection_link dbc) override;
  void reg() override;
  void reg(std::vector<std::string>& sql) override;
  void unreg() override;

  std::shared_ptr<brig::rowset> attributes(const frame& fr) override;
  std::shared_ptr<brig::rowset> drawing(const frame& fr) override;
  void draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter) override;
  bool has_spatial_index(const frame& fr) override;
  double native_scale(const frame& fr) override;
}; // layer_raster

#endif // LAYER_RASTER_H
