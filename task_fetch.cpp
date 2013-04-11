// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QEventLoop>
#include <QTime>
#include "provider.h"
#include "task_fetch.h"
#include "utilities.h"

QString task_fetch::get_string()
{
  return limited_text(m_sql.c_str(), false);
}

void task_fetch::do_run(QEventLoop& loop)
{
  using namespace std;

  size_t counter(0);
  QTime time; time.start();
  shared_ptr<rowset_model> mdl(new rowset_model());
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
    loop.processEvents();
    if (m_cancel) throw runtime_error("canceled");
    emit signal_progress(QString("rows: %1").arg(counter + 1));
    time.restart();
  }

  loop.processEvents();
  if (m_cancel) throw runtime_error("canceled");
  if (counter >= brig::PageSize) emit signal_progress(QString("in excess of %1 rows").arg(brig::PageSize));
  else emit signal_progress(QString("rows: %1").arg(counter));

  emit signal_rowset(mdl);
}
