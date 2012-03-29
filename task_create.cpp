// Andrew Naplavkov

#include <stdexcept>
#include <QString>
#include <Qt>
#include <vector>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_create.h"
#include "task_insert.h"
#include "utilities.h"

task_create::task_create(layer_link lr_from, connection_link dbc_to, const std::string& tbl, bool sql)
  : m_lr_from(lr_from), m_dbc_to(dbc_to), m_tbl(tbl), m_sql(sql)
{
}

void task_create::run(progress* prg)
{
  auto dbc_from(m_lr_from->get_connection());
  std::vector<std::string> sql;
  for (size_t level(0); level < m_lr_from->get_levels(); ++level)
  {
    auto tbl(m_lr_from->get_table_definition(level));
    m_dbc_to->create_check_mbr(tbl);
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
    m_dbc_to->create(tbl, sql);
  }
  layer_link lr_to(m_lr_from->create_result(m_dbc_to, m_tbl, sql));

  if (m_sql) emit signal_commands(m_dbc_to, sql);
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

    std::vector<insert_item> items;
    for (size_t level(0); level < m_lr_from->get_levels(); ++level)
    {
      auto tbl_from(m_lr_from->get_table_definition(level));
      auto tbl_to(lr_to->get_table_definition(level));
      for (auto col_to(std::begin(tbl_to.columns)); col_to != std::end(tbl_to.columns); ++col_to)
      {
        if (brig::database::VoidColumn == col_to->type) continue;
        auto col_from(std::find_if(std::begin(tbl_from.columns), std::end(tbl_from.columns), [&](const brig::database::column_definition& c){ return c.name == col_to->name; }));
        if (col_from == std::end(tbl_from.columns))
        {
          const QString name(QString::fromUtf8(col_to->name.c_str()));
          col_from = std::find_if(std::begin(tbl_from.columns), std::end(tbl_from.columns), [&](const brig::database::column_definition& c){ return 0 == name.compare(QString::fromUtf8(c.name.c_str()), Qt::CaseInsensitive); });
          if (col_from == std::end(tbl_from.columns)) continue;
        }

        insert_item item;
        item.level = int(level);
        item.column_to = col_to->name;
        item.column_from = col_from->name;
        items.push_back(item);
      }
    }

    task_insert::run(m_lr_from, lr_to, items, prg);
    emit signal_refresh(m_dbc_to);
  }
}
