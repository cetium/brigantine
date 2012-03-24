// Andrew Naplavkov

#include <brig/database/threaded_rowset.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "reproject.h"
#include "task_insert.h"
#include "utilities.h"

void task_insert::run(layer_link lr_from, layer_link lr_to, const insert_map& i_map, progress* prg)
{
  const int BatchInsert = 10000;

  if (lr_from->get_levels() != lr_to->get_levels()) throw std::runtime_error("insert error");

  size_t counter(0), batch(0);
  for (size_t level(0); level < lr_from->get_levels(); ++level)
  {
    auto tbl_from(lr_from->get_table_definition(level));
    auto tbl_to(lr_to->get_table_definition(level));
    reproject_map r_map;
    std::vector<brig::database::column_definition> param_cols;

    for (auto iter(std::begin(i_map)); iter != std::end(i_map); ++iter)
    {
      if (iter->level != level) continue;

      auto col_from(std::find_if
        ( std::begin(tbl_from.columns)
        , std::end(tbl_from.columns)
        , [&](const brig::database::column_definition& c){ return c.name == iter->column_from; })
        );

      auto col_to(std::find_if
        ( std::begin(tbl_to.columns)
        , std::end(tbl_to.columns)
        , [&](const brig::database::column_definition& c){ return c.name == iter->column_to; })
        );

      if (col_from == std::end(tbl_from.columns) || col_to == std::end(tbl_to.columns)) throw std::runtime_error("insert error");

      tbl_from.select_columns.push_back(col_from->name);
      tbl_to.select_columns.push_back(col_to->name);

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

      reproject_map_item item;
      item.column = int(param_cols.size());
      if (col_from->epsg != col_to->epsg)
      {
        item.pj_from = get_epsg(col_from->epsg);
        item.pj_to = get_epsg(col_to->epsg);
        r_map.push_back(item);
      }

      param_cols.push_back(*col_to);
    }

    auto rowset(lr_from->get_connection()->get_rowset(tbl_from));
    if (!r_map.empty()) rowset = std::make_shared<brig::database::threaded_rowset>(std::make_shared<reproject>(rowset, r_map));

    auto dbc_to(lr_to->get_connection());
    const std::string sql(dbc_to->insert(tbl_to));
    auto command(dbc_to->get_command());
    command->set_autocommit(false);
    std::vector<brig::database::variant> row;

    while (rowset->fetch(row))
    {
      ++counter;
      ++batch;

      if (!prg->step(counter)) return;
      command->exec(sql, row, param_cols);

      if (batch >= BatchInsert)
      {
        command->commit();
        if (!prg->step(counter)) return;
        batch = 0;
      }
    }

    if (batch > 0)
    {
      command->commit();
      if (!prg->step(counter)) return;
    }
    batch = 0;

    dbc_to->reset_table_definition(tbl_to.id);
  }
}
