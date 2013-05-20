// Andrew Naplavkov

#include <brig/boost/as_binary.hpp>
#include <brig/threaded_rowset.hpp>
#include <QTime>
#include <stdexcept>
#include <vector>
#include "rowset_ccw.h"
#include "layer.h"
#include "provider.h"
#include "rowset_transform.h"
#include "task_insert.h"
#include "transformer.h"
#include "utilities.h"

QString task_insert::get_string()
{
  return QString("inserting into '%1'").arg(m_lr_to->get_string(true));
}

void task_insert::run_impl()
{
  using namespace std;
  frame fr = get_frame();
  QTime time; time.start();
  if (m_lr_from->get_levels() != m_lr_to->get_levels()) throw runtime_error("level count error");
  for (size_t level(0); level < m_lr_from->get_levels(); ++level)
  {
    auto tbl_from(m_lr_from->get_table_def(level));
    auto tbl_to(m_lr_to->get_table_def(level));
    vector<int> ccw_cols;
    vector<rowset_transform::item> transform_items;

    for (const auto& item: m_items)
    {
      if (size_t(item.level) != level) continue;

      auto col_from(tbl_from[item.column_from]);
      auto col_to(tbl_to[item.column_to]);

      if (item.epsg > 0)
      {
        col_to->srid = item.epsg;
        col_to->epsg = item.epsg;
      }

      if (col_to->srid <= 0)
      {
        col_to->srid = col_from->srid;
        col_to->epsg = col_from->epsg;
      }

      if (col_to->type == brig::column_type::Geometry && m_ccw)
        ccw_cols.push_back( int(tbl_from.query_columns.size()) );

      if (col_from->epsg != col_to->epsg)
      {
        rowset_transform::item item;
        item.column = int(tbl_from.query_columns.size());
        item.tr = transformer(get_pj(*col_from), get_pj(*col_to));
        transform_items.push_back(item);
      }

      tbl_from.query_columns.push_back(col_from->name);
      tbl_to.query_columns.push_back(col_to->name);
    }

    if (m_view)
    {
      auto col_from(tbl_from[m_lr_from->get_geometry(level).qualifier]);
      QRectF rect(transformer(fr.get_pj(), get_pj(*col_from)).transform(fr.prepare_rect()));
      col_from->query_value = brig::boost::as_binary(rect_to_box(rect));
    }

    auto rowset(m_lr_from->get_provider()->select(tbl_from));
    if (!ccw_cols.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<rowset_ccw>(rowset, ccw_cols));
    if (!transform_items.empty()) rowset = make_shared<brig::threaded_rowset>(make_shared<rowset_transform>(rowset, transform_items));

    auto pvd_to(m_lr_to->get_provider());
    auto ins(pvd_to->get_inserter(tbl_to));
    vector<brig::variant> row;

    for (; rowset->fetch(row); ++m_counter)
    {
      ins->insert(row);
      if (time.elapsed() < BatchInterval) continue;

      ins->flush();
      time.restart();
      progress(QString("rows: %1").arg(m_counter));
    }

    ins->flush();
    time.restart();
    progress(QString("rows: %1").arg(m_counter));
  }
  m_lr_to->reset_table_defs();
}
