// Andrew Naplavkov

#ifndef LAYER_H
#define LAYER_H

#include <memory>
#include <QPainter>
#include <QString>
#include <Qt>
#include <string>
#include <vector>
#include "frame.h"
#include "projection.h"
#include "provider.h"
#include "provider_ptr.h"

class layer {
  provider_ptr m_pvd;

protected:
  brig::variant prepare_box(const frame& fr);

public:
  layer()  {}
  explicit layer(provider_ptr pvd) : m_pvd(pvd)  {}
  virtual ~layer()  {}
  virtual QString get_icon() = 0;

  virtual brig::identifier get_identifier() = 0;
  virtual size_t get_levels() = 0;
  virtual brig::identifier get_geometry(size_t lvl) = 0;
  virtual brig::table_def get_table_def(size_t lvl) = 0;
  virtual void reset_table_defs() = 0;
  virtual bool is_raster() = 0;

  virtual layer* fit(provider_ptr pvd) = 0;
  virtual void reg() = 0;
  virtual void reg(std::vector<std::string>& sql) = 0;
  virtual void unreg() = 0;

  virtual std::shared_ptr<brig::rowset> attributes(const frame& fr) = 0;
  virtual std::shared_ptr<brig::rowset> drawing(const frame& fr) = 0;
  virtual void draw(const std::vector<brig::variant>& row, const frame& fr, QPainter& painter) = 0;
  virtual bool has_spatial_index(const frame& fr) = 0;
  virtual double native_scale(const frame& fr) = 0;

  provider_ptr get_provider()  { return m_pvd; }
  QString get_string(bool full = false);
  projection get_pj();
  bool try_pj(projection& pj);
  bool try_view(brig::boost::box& box, projection& pj);
}; // layer

#endif // LAYER_H
