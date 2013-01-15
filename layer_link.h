// Andrew Naplavkov

#ifndef LAYER_LINK_H
#define LAYER_LINK_H

#include <memory>
#include <QMetaType>
#include <Qt>

class layer; // https://svn.boost.org/trac/boost/ticket/6687

class layer_link {
  std::shared_ptr<layer> m_lr;

public:
  bool m_checked;
  size_t m_order;

  explicit layer_link(layer* lr = 0);
  layer* operator ->() const;
  operator bool() const;
}; // layer_link

Q_DECLARE_METATYPE(layer_link)

#endif // LAYER_LINK_H
