// Andrew Naplavkov

#include "layer.h"
#include "layer_link.h"

layer_link::layer_link(layer* lr) : m_lr(lr), m_state(Qt::Unchecked), m_order(0)  {}
layer* layer_link::operator ->() const  { return m_lr.get(); }
layer_link::operator bool() const  { return bool(m_lr); }
