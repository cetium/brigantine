// Andrew Naplavkov

#include <brig/database/connection.hpp>
#include <brig/proj/epsg.hpp>
#include <memory>
#include <vector>

#ifndef REPROJECT_H
#define REPROJECT_H

struct reproject_map_item {
  int column;
  brig::proj::epsg pj_from, pj_to;
  reproject_map_item() : column(-1)  {}
}; // reproject_map_item

typedef std::vector<reproject_map_item> reproject_map;

class reproject : public brig::database::rowset {
  std::shared_ptr<brig::database::rowset> m_rs;
  reproject_map m_map;
public:
  reproject(std::shared_ptr<brig::database::rowset> rs, const reproject_map& map) : m_rs(rs), m_map(map)  {}
  virtual std::vector<std::string> columns()  { return m_rs->columns(); }
  virtual bool fetch(std::vector<brig::database::variant>& row);
}; // reproject

#endif // REPROJECT_H
