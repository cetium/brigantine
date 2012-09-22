// Andrew Naplavkov

#include <brig/boost/as_binary.hpp>
#include <QString>
#include <Qt>
#include <vector>
#include "connection.h"
#include "layer.h"
#include "progress.h"
#include "task_create.h"
#include "task_insert.h"
#include "utilities.h"

task_create::task_create(std::vector<layer_link> lrs_from, connection_link dbc_to, const std::string& name, bool sql)
  : m_lrs_from(lrs_from), m_dbc_to(dbc_to), m_name(lrs_from.size() == 1? name: ""), m_sql(sql)
{
}

void task_create::run(progress* prg)
{
  std::vector<std::string> sql;
  for (size_t lr(0); lr < m_lrs_from.size(); ++lr)
  {
    if (!m_sql) sql.clear();

    auto lr_from(m_lrs_from[lr]);
    std::string name(m_name.empty()? lr_from->get_identifier().name: m_name);
    auto dbc_from(lr_from->get_connection());
    for (size_t level(0); level < lr_from->get_levels(); ++level)
    {
      if (!prg->step()) return;

      auto tbl(lr_from->get_table_definition(level));
      m_dbc_to->create_check_mbr(tbl);
      for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
        if ( brig::database::Geometry == col->type
          && typeid(brig::blob_t) == col->query_value.type()
          && boost::get<brig::blob_t>(col->query_value).empty()
           )
        {
          brig::database::identifier id(tbl.id); id.qualifier = col->name;
          auto box(dbc_from->get_mbr(id, *col));
          dbc_from->set_mbr(id, box);
          if (!prg->step()) return;
          col->query_value = brig::boost::as_binary(box);
        }
      tbl.id.name = get_table_name(name, level);
      m_dbc_to->create(tbl, sql);
    }
    layer_link lr_to(lr_from->create_result(m_dbc_to, name, sql));

    if (m_sql) continue;

    {
      auto cmd(m_dbc_to->get_command());
      for (auto s(std::begin(sql)); s != std::end(sql); ++s)
      {
        cmd->exec(*s);
        if (!prg->step()) return;
      }
    }

    std::vector<insert_item> items;
    for (size_t level(0); level < lr_from->get_levels(); ++level)
    {
      auto tbl_from(lr_from->get_table_definition(level));
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
    task_insert::run(lr_from, lr_to, items, false, prg);
  }

  if (m_sql)
    emit signal_commands(m_dbc_to, sql);
  else
    emit signal_refresh(m_dbc_to);
}
