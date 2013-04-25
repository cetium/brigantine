// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QTime>
#include "provider.h"
#include "task_fetch.h"
#include "utilities.h"

QString task_fetch::get_string()
{
  return limited_text(m_sql.c_str(), false);
}

void task_fetch::run_impl()
{
  using namespace std;

  size_t counter(0);
  QTime time; time.start();
  shared_ptr<rowset_model> mdl(new rowset_model(0));
  auto cmd(m_pvd->get_command());
  cmd->exec(m_sql);
  mdl->m_columns = cmd->columns();
  vector<brig::variant> row;

  for (; cmd->fetch(row) && counter < brig::PageSize; ++counter)
  {
    vector<string> strs;
    for (auto itr(begin(row)); itr != end(row); ++itr)
      strs.push_back(brig::string_cast<char>(*itr));
    mdl->m_rows.push_back(move(strs));

    if (time.elapsed() < BatchInterval) continue;
    progress(QString("rows: %1").arg(counter + 1));
    time.restart();
  }

  if (counter >= brig::PageSize) progress(QString("in excess of %1 rows").arg(brig::PageSize));
  else progress(QString("rows: %1").arg(counter));

  emit signal_rowset(mdl);
}
