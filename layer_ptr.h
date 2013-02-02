// Andrew Naplavkov

#ifndef LAYER_PTR_H
#define LAYER_PTR_H

#include <memory>
#include <QMetaType>
#include <Qt>

class layer; // https://svn.boost.org/trac/boost/ticket/6687

class layer_ptr {
  std::shared_ptr<layer> m_lr;

public:
  bool m_checked;
  size_t m_order;

  explicit layer_ptr(layer* lr = 0);
  layer* operator ->() const;
  operator bool() const;
}; // layer_ptr

Q_DECLARE_METATYPE(layer_ptr)

#endif // LAYER_PTR_H
