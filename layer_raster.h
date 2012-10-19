// Andrew Naplavkov

#ifndef LAYER_RASTER_H
#define LAYER_RASTER_H

#include "layer.h"

class layer_raster : public layer {
  const brig::database::raster_pyramid m_raster;

  size_t get_level(const frame& fr) const;
  std::string get_raster_column(size_t level) const;

public:
  layer_raster(connection_link dbc, const brig::database::raster_pyramid& raster) : layer(dbc), m_raster(raster)  {}
  virtual QString get_icon()  { return ":/res/palette.png"; }

  virtual brig::database::identifier get_identifier()  { return m_raster.id; }
  virtual brig::database::identifier get_geometry()  { return m_raster.levels[0].geometry; }
  virtual size_t get_levels()  { return m_raster.levels.size(); }
  virtual brig::database::table_definition get_table_definition(size_t level);
  virtual void reset_table_definitions();
  virtual bool is_writable();

  virtual layer* reg(connection_link dbc, std::vector<std::string>& sql);
  virtual void unreg(std::vector<std::string>& sql);

  virtual size_t limit()  { return 100; }
  virtual bool has_spatial_index(const frame& fr);
  virtual std::shared_ptr<brig::database::rowset> attributes(const frame& fr);
  virtual std::shared_ptr<brig::database::rowset> drawing(const frame& fr, bool limited);
  virtual void draw(const std::vector<brig::database::variant>& row, const frame& fr, QPainter& painter);
}; // layer_raster

#endif // LAYER_RASTER_H
