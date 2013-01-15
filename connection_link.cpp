// Andrew Naplavkov

#include "connection.h"
#include "connection_link.h"

connection_link::connection_link(brig::connection* dbc, QString str, QString icon) : m_dbc(std::make_shared<connection>(dbc, str, icon))  {}
connection* connection_link::operator ->()  { return m_dbc.get(); }
bool connection_link::operator ==(const connection_link& r) const  { return m_dbc.get() == r.m_dbc.get(); }
bool connection_link::operator !=(const connection_link& r) const  { return m_dbc.get() != r.m_dbc.get(); }
connection_link::operator bool() const  { return bool(m_dbc); }
