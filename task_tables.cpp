// Andrew Naplavkov

#include "provider.h"
#include "task_tables.h"

QString task_tables::get_string()
{
  return QString("tables of '%1'").arg(m_pvd->get_string());
}

void task_tables::do_run(QEventLoop&)
{
  using namespace std;
  shared_ptr<rowset_model> mdl(new rowset_model());
  mdl->m_columns.push_back("SCHEMA");
  mdl->m_columns.push_back("TABLE");
  auto ids(m_pvd->get_tables());
  size_t counter(1);
  for (auto id(begin(ids)); id != end(ids); ++id, ++counter)
  {
    vector<string> strs;
    strs.push_back(id->schema);
    strs.push_back(id->name);
    mdl->m_rows.push_back(move(strs));
  }
  emit signal_rowset(mdl);
}
