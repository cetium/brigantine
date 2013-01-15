// Andrew Naplavkov

#include <brig/database/connection.hpp>
#include <memory>
#include <vector>

#ifndef COUNTER_CLOCKWISE_H
#define COUNTER_CLOCKWISE_H

// DB2 (geodetic regions): as you follow a polygon from vertex to vertex in the order defined, the area to the left is inside the polygon
// MS_SQL: the interior of the polygon in an ellipsoidal system is defined by the left-hand rule
// Oracle: counter clockwise for exterior rings, and clockwise for interior rings

class counter_clockwise : public brig::rowset {
  std::shared_ptr<brig::rowset> m_rs;
  std::vector<int> m_cols;
public:
  counter_clockwise(std::shared_ptr<brig::rowset> rs, const std::vector<int>& cols) : m_rs(rs), m_cols(cols)  {}
  std::vector<std::string> columns() override  { return m_rs->columns(); }
  bool fetch(std::vector<brig::variant>& row) override;
}; // counter_clockwise

#endif // COUNTER_CLOCKWISE_H
