// Andrew Naplavkov

#include "layer.h"
#include "layer_link.h"

layer_link::resource::resource(layer* lr) : m_lr(lr)  {}
layer_link::resource::~resource()  { delete m_lr; }

layer_link::layer_link(layer* lr) : m_state(Qt::Unchecked), m_order(0)  { m_link = std::make_shared<resource>(lr); }
layer* layer_link::operator ->() const  { return m_link->m_lr; }
layer_link::operator bool() const  { return m_link->m_lr != 0; }
