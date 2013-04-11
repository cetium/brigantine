// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QEventLoop>
#include <QTime>
#include "layer.h"
#include "provider.h"
#include "task_attributes.h"
#include "utilities.h"

QString task_attributes::get_string()
{
  return QString("attributes of '%1'").arg(m_lr->get_string(true));
}

void task_attributes::do_run(QEventLoop& loop)
{
  using namespace std;

  size_t counter(0);
  QTime time; time.start();
  shared_ptr<rowset_model> mdl(new rowset_model());
  auto rs(m_lr->attributes(m_fr));
  mdl->m_columns = rs->columns();
  vector<brig::variant> row;

  for (; rs->fetch(row) && counter < brig::PageSize; ++counter)
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
