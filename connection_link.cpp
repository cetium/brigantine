// Andrew Naplavkov

#include "connection.h"
#include "connection_link.h"

connection_link::resource::resource(connection* dbc) : m_dbc(dbc)  {}
connection_link::resource::~resource()  { delete m_dbc; }

connection_link::connection_link(connection* dbc)  { m_link = std::make_shared<resource>(dbc); }
connection* connection_link::operator ->()  { return m_link->m_dbc; }
bool connection_link::operator ==(const connection_link& r) const  { return m_link->m_dbc == r.m_link->m_dbc; }
bool connection_link::operator !=(const connection_link& r) const  { return m_link->m_dbc != r.m_link->m_dbc; }
connection_link::operator bool() const  { return m_link->m_dbc != 0; }
