// Andrew Naplavkov

#include <brig/proj/shared_pj.hpp>
#include <brig/rowset.hpp>
#include <memory>
#include <vector>

#ifndef REPROJECT_H
#define REPROJECT_H

struct reproject_item {
  int column;
  brig::proj::shared_pj pj_from, pj_to;
  reproject_item() : column(-1)  {}
}; // reproject_item

class reproject : public brig::rowset {
  std::shared_ptr<brig::rowset> m_rs;
  std::vector<reproject_item> m_items;
public:
  reproject(std::shared_ptr<brig::rowset> rs, const std::vector<reproject_item>& items) : m_rs(rs), m_items(items)  {}
  std::vector<std::string> columns() override  { return m_rs->columns(); }
  bool fetch(std::vector<brig::variant>& row) override;
}; // reproject

#endif // REPROJECT_H
