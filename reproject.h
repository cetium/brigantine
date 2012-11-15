// Andrew Naplavkov

#include <brig/database/connection.hpp>
#include <brig/proj/shared_pj.hpp>
#include <memory>
#include <vector>

#ifndef REPROJECT_H
#define REPROJECT_H

struct reproject_item {
  int column;
  brig::proj::shared_pj pj_from, pj_to;
  reproject_item() : column(-1)  {}
}; // reproject_item

class reproject : public brig::database::rowset {
  std::shared_ptr<brig::database::rowset> m_rs;
  std::vector<reproject_item> m_items;
public:
  reproject(std::shared_ptr<brig::database::rowset> rs, const std::vector<reproject_item>& items) : m_rs(rs), m_items(items)  {}
  virtual std::vector<std::string> columns()  { return m_rs->columns(); }
  virtual bool fetch(std::vector<brig::database::variant>& row);
}; // reproject

#endif // REPROJECT_H
