// Andrew Naplavkov

#ifndef PROJECTION_H
#define PROJECTION_H

#include <string>

class projection {
  int m_epsg;
  std::string m_def;
public:
  projection() : m_epsg(-1)  {}
  explicit projection(int epsg) : m_epsg(epsg)  {}
  explicit projection(const std::string& def) : m_epsg(-1), m_def(def)  {}
  int get_epsg() const  { return m_epsg; }
  std::string get_def() const  { return m_def; }
  bool operator ==(const projection& r) const  { return m_epsg < 0? m_def.compare(r.m_def) == 0: m_epsg == r.m_epsg; }
  bool operator !=(const projection& r) const  { return !operator ==(r); }
}; // projection

#endif // PROJECTION_H
