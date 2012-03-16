// Andrew Naplavkov

#include "layer.h"
#include "layer_link.h"

layer_link::resource::resource() : lr(0)  {}
layer_link::resource::~resource()  { delete lr; }
layer_link::layer_link() : m_state(Qt::Unchecked), m_order(0)  { link = std::make_shared<resource>(); }
bool layer_link::operator <(const layer_link& r) const  { return m_order < r.m_order; }
layer* layer_link::operator ->() const  { return link->lr; }
