// Andrew Naplavkov

#ifndef CONNECTION_LINK_H
#define CONNECTION_LINK_H

#include <memory>

class connection; // https://svn.boost.org/trac/boost/ticket/6687

struct connection_link
{
  struct resource {
    connection* dbc;
    resource();
    ~resource();
  }; // resource

  std::shared_ptr<resource> link;

  connection_link();
  connection* operator ->();
}; // connection_link

#endif // CONNECTION_LINK_H
