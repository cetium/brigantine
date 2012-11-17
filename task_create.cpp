// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/as_binary.hpp>
#include <iterator>
#include <Qt>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_create.h"
#include "task_insert.h"
#include "utilities.h"

task_create::task_create(std::vector<layer_link> lrs_from, connection_link dbc_to, bool sql)
  : m_lrs_from(lrs_from), m_dbc_to(dbc_to), m_sql(sql)
{
}

void task_create::run(progress* prg)
{
  using namespace std;
  using namespace brig::database;

  size_t counter(0);
  vector<string> sql;
  for (size_t lr(0); lr < m_lrs_from.size(); ++lr)
  {
    if (!m_sql) sql.clear();
    auto lr_from(m_lrs_from[lr]);
    auto dbc_from(lr_from->get_connection());
    vector<insert_item> items;
    vector<string> reg;
    layer_link lr_to(lr_from->reg(m_dbc_to, reg));

    for (size_t lvl(0); lvl < lr_from->get_levels(); ++lvl)
    {
      if (!prg->step(counter)) return;
      auto tbl_from(lr_from->get_table_definition(lvl));
      auto tbl_to(tbl_from);
      tbl_to.id.name = lr_to->get_geometry(lvl).name;
      tbl_to = m_dbc_to->fit_to_create(tbl_to);

      for (size_t col(0), cols(min<>(tbl_from.columns.size(), tbl_to.columns.size())); col < cols; ++col)
      {
        const column_definition& col_from(tbl_from.columns[col]);
        column_definition& col_to(tbl_to.columns[col]);

        insert_item item;
        item.level = int(lvl);
        item.column_to = col_to.name;
        item.column_from = col_from.name;
        items.push_back(item);

        if ( Geometry == col_to.type
          && typeid(brig::blob_t) == col_to.query_value.type()
          && boost::get<brig::blob_t>(col_to.query_value).empty()
           )
        {
          auto box(dbc_from->get_mbr(tbl_from, col_from.name));
          identifier id = tbl_from.id; id.qualifier = col_from.name;
          dbc_from->set_mbr(id, box);
          if (!prg->step(counter)) return;
          col_to.query_value = brig::boost::as_binary(box);
        }
      }

      m_dbc_to->create(tbl_to, sql);
    }
    sql.insert(end(sql), begin(reg), end(reg));

    if (m_sql) continue;

    {
      auto cmd(m_dbc_to->get_command());
      for (auto s(begin(sql)); s != end(sql); ++s)
      {
        cmd->exec(*s);
        if (!prg->step(counter)) return;
      }
    }

    task_insert::run(lr_from, lr_to, items, false, counter, prg);
  }

  if (m_sql)
    emit signal_commands(m_dbc_to, sql);
  else
    emit signal_refresh(m_dbc_to);
}
