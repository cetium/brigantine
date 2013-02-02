// Andrew Naplavkov

#include <algorithm>
#include <brig/boost/as_binary.hpp>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <iterator>
#include <Qt>
#include <stdexcept>
#include "layer.h"
#include "progress.h"
#include "provider.h"
#include "task_create.h"
#include "task_insert.h"
#include "utilities.h"

task_create::task_create(std::vector<layer_ptr> lrs_from, provider_ptr pvd_to, bool sql, bool view)
  : m_lrs_from(lrs_from), m_pvd_to(pvd_to), m_sql(sql), m_view(view)
{
}

void task_create::run(progress* prg)
{
  using namespace std;

  size_t counter(0);
  vector<string> sql;
  for (size_t lr(0); lr < m_lrs_from.size(); ++lr)
  {
    if (!m_sql) sql.clear();
    auto lr_from(m_lrs_from[lr]);
    auto pvd_from(lr_from->get_provider());
    vector<insert_item> items;
    layer_ptr lr_to(lr_from->fit(m_pvd_to));

    for (size_t lvl(0); lvl < lr_from->get_levels(); ++lvl)
    {
      if (!prg->step(counter)) return;
      auto tbl_from(lr_from->get_table_def(lvl));
      auto tbl_to(lr_to->get_table_def(lvl));

      for (size_t col(0), cols(min<>(tbl_from.columns.size(), tbl_to.columns.size())); col < cols; ++col)
      {
        const brig::column_def& col_from(tbl_from.columns[col]);
        brig::column_def& col_to(tbl_to.columns[col]);

        insert_item item;
        item.level = int(lvl);
        item.column_to = col_to.name;
        item.column_from = col_from.name;
        items.push_back(item);

        if ( brig::Geometry == col_to.type
          && typeid(brig::blob_t) == col_to.query_value.type()
           )
        {
          using namespace brig::boost;

          box mbr;
          if (boost::get<brig::blob_t>(col_to.query_value).empty())
          {
            mbr = pvd_from->get_mbr(tbl_from, col_from.name);
            brig::identifier id = tbl_from.id; id.qualifier = col_from.name;
            pvd_from->set_mbr(id, mbr);
            if (!prg->step(counter)) return;
          }
          else
            mbr = envelope(geom_from_wkb(boost::get<brig::blob_t>(col_to.query_value)));

          if (m_view)
          {
            QRectF rect = ::transform(m_fr.prepare_rect(), m_fr.get_pj(), ::get_pj(col_to));
            box intersetion;
            if (!boost::geometry::intersection(rect_to_box(rect), mbr, intersetion))
               throw runtime_error("current extent error");
            col_to.query_value = as_binary(intersetion);
          }
          else
            col_to.query_value = as_binary(mbr);
        }
      }

      if (m_sql)
        m_pvd_to->create(tbl_to, sql);
      else
        m_pvd_to->create(tbl_to);
    }

    if (m_sql)
      lr_to->reg(sql);
    else
    {
      lr_to->reg();
      task_insert tsk(lr_from, lr_to, items, false, m_view);
      tsk.m_counter = counter;
      tsk.set_frame(m_fr);
      tsk.run(prg);
      counter = tsk.m_counter;
    }
  }

  if (m_sql)
    emit signal_sql(m_pvd_to, sql);
  else
    emit signal_refresh(m_pvd_to);
}
