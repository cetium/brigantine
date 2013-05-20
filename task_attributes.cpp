// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QTime>
#include "layer.h"
#include "provider.h"
#include "task_attributes.h"
#include "utilities.h"

QString task_attributes::get_string()
{
  return QString("attributes of '%1'").arg(m_lr->get_string(true));
}

void task_attributes::run_impl()
{
  using namespace std;

  size_t counter(0);
  QTime time; time.start();
  shared_ptr<rowset_model> mdl(new rowset_model(0));
  auto rs(m_lr->attributes(get_frame()));
  mdl->m_columns = rs->columns();
  vector<brig::variant> row;

  for (; rs->fetch(row) && counter < brig::PageSize; ++counter)
  {
    vector<string> strs;
    for (const auto& v: row) strs.push_back(brig::string_cast<char>(v));
    mdl->m_rows.push_back(move(strs));

    if (time.elapsed() < BatchInterval) continue;
    progress(QString("rows: %1").arg(counter + 1));
    time.restart();
  }

  if (counter >= brig::PageSize) progress(QString("in excess of %1 rows").arg(brig::PageSize));
  else progress(QString("rows: %1").arg(counter));

  emit signal_rowset(mdl);
}
