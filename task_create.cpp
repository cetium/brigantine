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
#include "provider.h"
#include "task_create.h"
#include "task_insert.h"
#include "transformer.h"
#include "utilities.h"

QString task_create::get_string()
{
  return QString("creating in '%1'").arg(m_pvd_to->get_string());
}

void task_create::run_impl()
{
  class inserter : public task_insert {
    task* m_owner;
  public:
    inserter
      ( layer_ptr lr_from, layer_ptr lr_to, const std::vector<insert_item>& items, bool ccw, bool view, size_t counter
      , task* owner
      )
      : task_insert(lr_from, lr_to, items, ccw, view, counter), m_owner(owner)
      {}
    void progress(QString msg) override  { m_owner->progress(msg); }
  };

  using namespace std;

  frame fr = get_frame();
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

        if ( brig::column_type::Geometry == col_to.type
          && typeid(brig::blob_t) == col_to.query_value.type()
           )
        {
          using namespace brig::boost;

          box mbr;
          if (boost::get<brig::blob_t>(col_to.query_value).empty())
          {
            tbl_from.query_columns.push_back(col_from.name);
            mbr = pvd_from->get_extent(tbl_from);
            tbl_from.query_columns.clear();
            brig::identifier id = tbl_from.id; id.qualifier = col_from.name;
            pvd_from->set_extent(id, mbr);
          }
          else
            mbr = envelope(geom_from_wkb(boost::get<brig::blob_t>(col_to.query_value)));

          if (m_view)
          {
            QRectF rect = transformer(fr.get_pj(), ::get_pj(col_to)).transform(fr.prepare_rect());
            box intersetion;
            if (!boost::geometry::intersection(rect_to_box(rect), mbr, intersetion))
               throw runtime_error("current extent error");
            col_to.query_value = as_binary(intersetion);
          }
          else
            col_to.query_value = as_binary(mbr);
        }
      }

      if (m_sql) m_pvd_to->create(tbl_to, sql);
      else m_pvd_to->create(tbl_to);
    }

    if (m_sql)
      lr_to->reg(sql);
    else
    {
      lr_to->reg();
      inserter tsk(lr_from, lr_to, items, false, m_view, counter, this);
      tsk.set_frame(fr);
      tsk.run_impl();
      counter = tsk.get_counter();
    }
  }

  if (m_sql) emit signal_sql(m_pvd_to, sql);
  else emit signal_refresh(m_pvd_to);
}
