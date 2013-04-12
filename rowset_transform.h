// Andrew Naplavkov

#include <brig/rowset.hpp>
#include <memory>
#include <vector>
#include "transformer.h"

#ifndef ROWSET_TRANSFORM_H
#define ROWSET_TRANSFORM_H

class rowset_transform : public brig::rowset {
public:
  struct item {
    int column;
    transformer tr;
    item() : column(-1)  {}
  }; // item
private:
  std::shared_ptr<brig::rowset> m_rs;
  std::vector<item> m_items;
public:
  rowset_transform(std::shared_ptr<brig::rowset> rs, const std::vector<item>& items) : m_rs(rs), m_items(items)  {}
  std::vector<std::string> columns() override  { return m_rs->columns(); }
  bool fetch(std::vector<brig::variant>& row) override;
}; // rowset_transform

#endif // ROWSET_TRANSFORM_H
