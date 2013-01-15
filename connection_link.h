// Andrew Naplavkov

#ifndef CONNECTION_LINK_H
#define CONNECTION_LINK_H

#include <memory>
#include <QMetaType>
#include <QString>

// https://svn.boost.org/trac/boost/ticket/6687
namespace brig { struct connection; }
class connection;

class connection_link {
  std::shared_ptr<connection> m_dbc;

public:
  connection_link()  {}
  connection_link(brig::connection* dbc, QString str, QString icon);
  connection* operator ->();
  bool operator ==(const connection_link& r) const;
  bool operator !=(const connection_link& r) const;
  operator bool() const;
}; // connection_link

Q_DECLARE_METATYPE(connection_link)

#endif // CONNECTION_LINK_H
