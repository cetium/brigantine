// Andrew Naplavkov

#ifndef CONNECTION_LINK_H
#define CONNECTION_LINK_H

#include <memory>
#include <QMetaType>
#include <QString>

// https://svn.boost.org/trac/boost/ticket/6687
namespace brig { struct provider; }
class provider;

class provider_ptr {
  std::shared_ptr<provider> m_pvd;

public:
  provider_ptr()  {}
  provider_ptr(brig::provider* pvd, QString str, QString icon);
  provider* operator ->();
  bool operator ==(const provider_ptr& r) const;
  bool operator !=(const provider_ptr& r) const;
  operator bool() const;
}; // provider_ptr

Q_DECLARE_METATYPE(provider_ptr)

#endif // CONNECTION_LINK_H
