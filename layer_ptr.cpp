// Andrew Naplavkov

#include "layer.h"
#include "layer_ptr.h"

layer_ptr::layer_ptr(layer* lr) : m_lr(lr), m_checked(false), m_order(0)  {}
layer* layer_ptr::operator ->() const  { return m_lr.get(); }
layer_ptr::operator bool() const  { return bool(m_lr); }
