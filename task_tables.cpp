// Andrew Naplavkov

#include "provider.h"
#include "task_tables.h"

QString task_tables::get_string()
{
  return QString("tables of '%1'").arg(m_pvd->get_string());
}

void task_tables::run_impl()
{
  using namespace std;
  shared_ptr<rowset_model> mdl(new rowset_model(0));
  mdl->m_columns.push_back("SCHEMA");
  mdl->m_columns.push_back("TABLE");
  for (const auto& id: m_pvd->get_tables())
  {
    vector<string> strs;
    strs.push_back(id.schema);
    strs.push_back(id.name);
    mdl->m_rows.push_back(move(strs));
  }
  progress( QString("tables: %1").arg(mdl->m_rows.size()) );
  emit signal_rowset(mdl);
}
