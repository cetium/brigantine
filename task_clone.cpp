// Andrew Naplavkov

#include <stdexcept>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_clone.h"
#include "task_insert.h"
#include "utilities.h"

task_clone::task_clone(layer_link lr_from, connection_link dbc_to, const std::string& tbl, bool sql)
  : m_lr_from(lr_from), m_dbc_to(dbc_to), m_tbl(tbl), m_sql(sql)
{
}

void task_clone::run(progress* prg)
{
  auto dbc_from(m_lr_from->get_connection());
  std::vector<std::string> sql;

  for (size_t level(0); level < m_lr_from->get_levels(); ++level)
  {
    auto tbl(m_lr_from->get_table_definition(level));
    m_dbc_to->clone_start(tbl);
    for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
      if (typeid(bool) == col->mbr.type() && boost::get<bool>(col->mbr))
      {
        brig::database::identifier id(tbl.id); id.qualifier = col->name;
        auto box(dbc_from->get_mbr(id, *col));
        dbc_from->set_mbr(id, box);
        if (!prg->step()) return;
        col->mbr = box;
      }
    tbl.id.name = get_table_name(m_tbl, level);
    m_dbc_to->clone(tbl, sql);
  }

  layer_link lr_to(m_lr_from->clone_finish(m_dbc_to, m_tbl, sql));

  if (m_sql)
    emit signal_commands(m_dbc_to, sql);
  else
  {
    {
      auto cmd(m_dbc_to->get_command());
      for (auto s(std::begin(sql)); s != std::end(sql); ++s)
      {
        cmd->exec(*s);
        if (!prg->step()) return;
      }
    }

    insert_map map;
    for (size_t level(0); level < m_lr_from->get_levels(); ++level)
    {
      auto tbl_from(m_lr_from->get_table_definition(level));
      auto tbl_to(lr_to->get_table_definition(level));
      for (auto col_to(std::begin(tbl_to.columns)); col_to != std::end(tbl_to.columns); ++col_to)
      {
        if (brig::database::VoidColumn == col_to->type) throw std::runtime_error("clone error");
        auto col_from(std::find_if(std::begin(tbl_from.columns), std::end(tbl_from.columns), [&](const brig::database::column_definition& c){ return c.name == col_to->name; }));
        if (col_from == std::end(tbl_from.columns))
        {
          const std::string lower(brig::unicode::transform<std::string>(col_to->name, brig::unicode::lower_case));
          col_from = std::find_if(std::begin(tbl_from.columns), std::end(tbl_from.columns), [&](const brig::database::column_definition& c){ return brig::unicode::transform<std::string>(c.name, brig::unicode::lower_case) == lower; });
          if (col_from == std::end(tbl_from.columns)) throw std::runtime_error("clone error");
        }

        insert_map_item item;
        item.level = int(level);
        item.column_to = col_to->name;
        item.column_from = col_from->name;
        map.push_back(item);
      }
    }

    task_insert::run(m_lr_from, lr_to, map, prg);
    emit signal_refresh(m_dbc_to);
  }
}
