// Andrew Naplavkov

#include <brig/database/connection.hpp>
#include <memory>
#include <vector>

#ifndef COUNTER_CLOCKWISE_H
#define COUNTER_CLOCKWISE_H

// DB2 (geodetic regions: as you follow a polygon from vertex to vertex in the order defined, the area to the left is inside the polygon
// MS_SQL: the interior of the polygon in an ellipsoidal system is defined by the left-hand rule
// Oracle: counter clockwise for exterior rings, and clockwise for interior rings

class counter_clockwise : public brig::database::rowset {
  std::shared_ptr<brig::database::rowset> m_rs;
  std::vector<int> m_cols;
public:
  counter_clockwise(std::shared_ptr<brig::database::rowset> rs, const std::vector<int>& cols) : m_rs(rs), m_cols(cols)  {}
  virtual std::vector<std::string> columns()  { return m_rs->columns(); }
  virtual bool fetch(std::vector<brig::database::variant>& row);
}; // counter_clockwise

#endif // COUNTER_CLOCKWISE_H
