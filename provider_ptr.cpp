// Andrew Naplavkov

#include "provider.h"
#include "provider_ptr.h"

provider_ptr::provider_ptr(brig::provider* pvd, QString str, QString icon) : m_pvd(std::make_shared<provider>(pvd, str, icon))  {}
provider* provider_ptr::operator ->()  { return m_pvd.get(); }
bool provider_ptr::operator ==(const provider_ptr& r) const  { return m_pvd.get() == r.m_pvd.get(); }
bool provider_ptr::operator !=(const provider_ptr& r) const  { return m_pvd.get() != r.m_pvd.get(); }
provider_ptr::operator bool() const  { return bool(m_pvd); }
