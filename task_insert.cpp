// Andrew Naplavkov

#include <brig/boost/as_binary.hpp>
#include <brig/threaded_rowset.hpp>
#include <QTime>
#include <stdexcept>
#include <vector>
#include "connection.h"
#include "counter_clockwise.h"
#include "layer.h"
#include "progress.h"
#include "reproject.h"
#include "task_insert.h"
#include "utilities.h"

void task_insert::run(progress* prg)
{
  using namespace std;

  if (m_lr_from->get_levels() != m_lr_to->get_levels()) throw runtime_error("level count error");

  for (size_t level(0); level < m_lr_from->get_levels(); ++level)
  {
    auto tbl_from(m_lr_from->get_table_definition(level));
    auto tbl_to(m_lr_to->get_table_definition(level));
    vector<int> ccw_cols;
    vector<reproject_item> reproject_items;

    for (auto iter(begin(m_items)); iter != end(m_items); ++iter)
    {
      if (size_t(iter->level) != level) continue;

      auto col_from(tbl_from[iter->column_from]);
      auto col_to(tbl_to[iter->column_to]);

      if (iter->epsg > 0)
      {
        col_to->srid = iter->epsg;
        col_to->epsg = iter->epsg;
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

    auto rowset(m_lr_from->get_connection()->select(tbl_from));
    if (!ccw_cols.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<counter_clockwise>(rowset, ccw_cols));
    if (!reproject_items.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<reproject>(rowset, reproject_items));

    auto dbc_to(m_lr_to->get_connection());
    auto ins(dbc_to->get_inserter(tbl_to));
    vector<brig::variant> row;
    QTime time; time.start();

    for (; rowset->fetch(row); ++m_counter)
    {
      if (!prg->step(m_counter)) return;
      ins->insert(row);
      if (time.elapsed() > BatchInterval)
      {
        ins->flush();
        if (!prg->step(m_counter)) return;
        time.restart();
      }
    }
    ins->flush();
    prg->step(m_counter);
  }

  m_lr_to->reset_table_definitions();
}
