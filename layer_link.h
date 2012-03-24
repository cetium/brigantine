// Andrew Naplavkov

#ifndef LAYER_LINK_H
#define LAYER_LINK_H

#include <memory>
#include <Qt>

class layer; // https://svn.boost.org/trac/boost/ticket/6687

class layer_link
{
  struct resource {
    layer* m_lr;
    explicit resource(layer* lr);
    ~resource();
  }; // resource

  std::shared_ptr<resource> m_link;

public:
  Qt::CheckState m_state;
  size_t m_order;

  explicit layer_link(layer* lr = 0);
  layer* operator ->() const;
  operator bool() const;
}; // layer_link

#endif // LAYER_LINK_H
