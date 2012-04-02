// Andrew Naplavkov

#ifndef CONNECTION_LINK_H
#define CONNECTION_LINK_H

#include <memory>

class connection; // https://svn.boost.org/trac/boost/ticket/6687

class connection_link {
  std::shared_ptr<connection> m_dbc;

public:
  explicit connection_link(connection* dbc = 0);
  connection* operator ->();
  bool operator ==(const connection_link& r) const;
  bool operator !=(const connection_link& r) const;
  operator bool() const;
}; // connection_link

#endif // CONNECTION_LINK_H
