// Andrew Naplavkov

#include "connection.h"
#include "connection_link.h"

connection_link::resource::resource() : dbc(0)  {}
connection_link::resource::~resource()  { delete dbc; }
connection_link::connection_link()  { link = std::make_shared<resource>(); }
bool connection_link::operator ==(const connection_link& r) const  { return link.get() == r.link.get(); }
connection* connection_link::operator ->()  { return link->dbc; }
