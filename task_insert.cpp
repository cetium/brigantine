// Andrew Naplavkov

#include <brig/boost/as_binary.hpp>
#include <brig/threaded_rowset.hpp>
#include <stdexcept>
#include <vector>
#include "counter_clockwise.h"
#include "layer.h"
#include "provider.h"
#include "reproject.h"
#include "task_insert.h"
#include "utilities.h"

QString task_insert::get_string()
{
  return QString("inserting into '%1'").arg(m_lr_to->get_string(true));
}

void task_insert::do_run(QTime& time, size_t& counter, QEventLoop& loop, bool& cancel)
{
  using namespace std;
  if (m_lr_from->get_levels() != m_lr_to->get_levels()) throw runtime_error("level count error");
  for (size_t level(0); level < m_lr_from->get_levels(); ++level)
  {
    auto tbl_from(m_lr_from->get_table_def(level));
    auto tbl_to(m_lr_to->get_table_def(level));
    vector<int> ccw_cols;
    vector<reproject_item> reproject_items;

    for (auto itr(begin(m_items)); itr != end(m_items); ++itr)
    {
      if (size_t(itr->level) != level) continue;

      auto col_from(tbl_from[itr->column_from]);
      auto col_to(tbl_to[itr->column_to]);

      if (itr->epsg > 0)
      {
        col_to->srid = itr->epsg;
        col_to->epsg = itr->epsg;
      }

      if (col_to->srid <= 0)
      {
        col_to->srid = col_from->srid;
        col_to->epsg = col_from->epsg;
      }

      if (col_to->type == brig::Geometry && m_ccw)
        ccw_cols.push_back( int(tbl_from.query_columns.size()) );

      if (col_from->epsg != col_to->epsg)
      {
        reproject_item item;
        item.column = int(tbl_from.query_columns.size());
        item.pj_from = get_pj(*col_from);
        item.pj_to = get_pj(*col_to);
        reproject_items.push_back(item);
      }

      tbl_from.query_columns.push_back(col_from->name);
      tbl_to.query_columns.push_back(col_to->name);
    }

    if (m_view)
    {
      auto col_from(tbl_from[m_lr_from->get_geometry(level).qualifier]);
      QRectF rect(transform(m_fr.prepare_rect(), m_fr.get_pj(), get_pj(*col_from)));
      col_from->query_value = brig::boost::as_binary(rect_to_box(rect));
    }

    auto rowset(m_lr_from->get_provider()->select(tbl_from));
    if (!ccw_cols.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<counter_clockwise>(rowset, ccw_cols));
    if (!reproject_items.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<reproject>(rowset, reproject_items));

    auto pvd_to(m_lr_to->get_provider());
    auto ins(pvd_to->get_inserter(tbl_to));
    vector<brig::variant> row;

    for (; rowset->fetch(row); ++counter)
    {
      ins->insert(row);
      if (time.elapsed() < BatchInterval) continue;

      loop.processEvents();
      if (cancel) throw runtime_error("canceled");
      ins->flush();
      time.restart();
      emit signal_progress(QString("rows: %1").arg(counter));
    }

    loop.processEvents();
    if (cancel) throw runtime_error("canceled");
    ins->flush();
    time.restart();
    emit signal_progress(QString("rows: %1").arg(counter));
  }
  m_lr_to->reset_table_defs();
}

void task_insert::do_run(QEventLoop& loop)
{
  QTime time; time.start();
  size_t counter;
  do_run(time, counter, loop, m_cancel);
}
