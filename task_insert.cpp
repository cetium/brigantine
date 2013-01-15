// Andrew Naplavkov

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

void task_insert::run(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& insert_items, bool ccw, size_t& counter, progress* prg)
{
  if (lr_from->get_levels() != lr_to->get_levels()) throw std::runtime_error("insert error");

  for (size_t level(0); level < lr_from->get_levels(); ++level)
  {
    auto tbl_from(lr_from->get_table_definition(level));
    auto tbl_to(lr_to->get_table_definition(level));
    std::vector<int> counter_clockwise_cols;
    std::vector<reproject_item> reproject_items;

    for (auto iter(std::begin(insert_items)); iter != std::end(insert_items); ++iter)
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

      if (col_to->type == brig::Geometry && ccw)
        counter_clockwise_cols.push_back( int(tbl_from.query_columns.size()) );

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

    auto rowset(lr_from->get_connection()->select(tbl_from));
    if (!counter_clockwise_cols.empty()) rowset = std::make_shared<brig::threaded_rowset>(std::make_shared<counter_clockwise>(rowset, counter_clockwise_cols));
    if (!reproject_items.empty()) rowset = std::make_shared<brig::threaded_rowset>(std::make_shared<reproject>(rowset, reproject_items));

    auto dbc_to(lr_to->get_connection());
    auto ins(dbc_to->get_inserter(tbl_to));
    std::vector<brig::variant> row;
    QTime time; time.start();

    for (; rowset->fetch(row); ++counter)
    {
      if (!prg->step(counter)) return;
      ins->insert(row);
      if (time.elapsed() > BatchInterval)
      {
        ins->flush();
        if (!prg->step(counter)) return;
        time.restart();
      }
    }
    ins->flush();
    prg->step(counter);
  }

  lr_to->reset_table_definitions();
}

void task_insert::run(progress* prg)
{
  size_t counter(0);
  run(m_lr_from, m_lr_to, m_items, m_ccw, counter, prg);
}
