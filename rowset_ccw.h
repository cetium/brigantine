// Andrew Naplavkov

#include <brig/rowset.hpp>
#include <memory>
#include <vector>

#ifndef ROWSET_CCW_H
#define ROWSET_CCW_H

// DB2 (geodetic regions): as you follow a polygon from vertex to vertex in the order defined, the area to the left is inside the polygon
// MS_SQL: the interior of the polygon in an ellipsoidal system is defined by the left-hand rule
// Oracle: counter clockwise for exterior rings, and clockwise for interior rings

class rowset_ccw : public brig::rowset {
  std::shared_ptr<brig::rowset> m_rs;
  std::vector<int> m_cols;
public:
  rowset_ccw(std::shared_ptr<brig::rowset> rs, const std::vector<int>& cols) : m_rs(rs), m_cols(cols)  {}
  std::vector<std::string> columns() override  { return m_rs->columns(); }
  bool fetch(std::vector<brig::variant>& row) override;
}; // rowset_ccw

#endif // ROWSET_CCW_H
