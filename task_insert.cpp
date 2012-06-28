// Andrew Naplavkov

#include <brig/database/threaded_rowset.hpp>
#include <QTime>
#include <stdexcept>
#include <string>
#include <vector>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "reproject.h"
#include "task_insert.h"
#include "utilities.h"

void task_insert::run(layer_link lr_from, layer_link lr_to, const std::vector<insert_item>& insert_items, progress* prg)
{
  if (lr_from->get_levels() != lr_to->get_levels()) throw std::runtime_error("insert error");

  size_t counter(0);
  for (size_t level(0); level < lr_from->get_levels(); ++level)
  {
    auto tbl_from(lr_from->get_table_definition(level));
    auto tbl_to(lr_to->get_table_definition(level));
    std::vector<reproject_item> reproject_items;
    std::vector<brig::database::column_definition> params;

    for (auto iter(std::begin(insert_items)); iter != std::end(insert_items); ++iter)
    {
      if (size_t(iter->level) != level) continue;

      auto col_from(tbl_from[iter->column_from]);
      auto col_to(tbl_to[iter->column_to]);

      tbl_from.query_columns.push_back(col_from->name);
      tbl_to.query_columns.push_back(col_to->name);

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

      reproject_item item;
      item.column = int(params.size());
      if (col_from->epsg != col_to->epsg)
      {
        item.pj_from = get_epsg(col_from->epsg);
        item.pj_to = get_epsg(col_to->epsg);
        reproject_items.push_back(item);
      }

      params.push_back(*col_to);
    }

    auto rowset(lr_from->get_connection()->select(tbl_from));
    if (!reproject_items.empty()) rowset = std::make_shared<brig::database::threaded_rowset>(std::make_shared<reproject>(rowset, reproject_items));

    auto dbc_to(lr_to->get_connection());
    const std::string sql(dbc_to->insert(tbl_to));
    auto command(dbc_to->get_command());
    command->set_autocommit(false);
    std::vector<brig::database::variant> row;
    QTime time; time.start();

    for (; rowset->fetch(row); ++counter)
    {
      if (!prg->step(counter)) return;
      if (row.size() != params.size()) throw std::runtime_error("insert error");
      for (size_t i(0); i < params.size(); ++ i)
        boost::swap(row[i], params[i].query_value);
      command->exec(sql, params);
      if (time.elapsed() > BatchInterval)
      {
        command->commit();
        if (!prg->step(counter)) return;
        time.restart();
      }
    }
    command->commit();
    prg->step(counter);

    dbc_to->reset_table_definition(tbl_to.id);
  }
}
