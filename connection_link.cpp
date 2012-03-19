// Andrew Naplavkov

#include "connection.h"
#include "connection_link.h"

connection_link::resource::resource() : dbc(0)  {}
connection_link::resource::~resource()  { delete dbc; }
connection_link::connection_link()  { link = std::make_shared<resource>(); }
connection* connection_link::operator ->()  { return link->dbc; }
