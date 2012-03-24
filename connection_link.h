// Andrew Naplavkov

#ifndef CONNECTION_LINK_H
#define CONNECTION_LINK_H

#include <memory>

class connection; // https://svn.boost.org/trac/boost/ticket/6687

class connection_link
{
  struct resource {
    connection* m_dbc;
    explicit resource(connection* dbc);
    ~resource();
  }; // resource

  std::shared_ptr<resource> m_link;

public:
  explicit connection_link(connection* dbc = 0);
  connection* operator ->();
  bool operator ==(const connection_link& r) const;
  bool operator !=(const connection_link& r) const;
  operator bool() const;
}; // connection_link

#endif // CONNECTION_LINK_H
