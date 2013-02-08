// Andrew Naplavkov

#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include <memory>
#include <QAction>
#include <QModelIndex>
#include <QPoint>
#include <QString>
#include <QTreeView>
#include <QWidget>
#include <vector>
#include "task.h"
#include "task_connect.h"
#include "tree_model.h"

class tree_view : public QTreeView {
  Q_OBJECT

  tree_model m_mdl;
  QModelIndex m_idx_menu;
  std::vector<layer_ptr> m_lrs_copy;

  QAction
    *m_connect_mysql_act,
    *m_connect_odbc_act,
    *m_connect_oracle_act,
    *m_connect_osm_act,
    *m_connect_postgres_act,
    *m_open_file_act,
    *m_new_file_act,
    *m_copy_checked_act,
    *m_refresh_act,
    *m_sql_console_act,
    *m_paste_layers_act,
    *m_disconnect_act,
    *m_zoom_to_fit_act,
    *m_use_projection_act,
    *m_snap_to_pixels_act,
    *m_attributes_act,
    *m_copy_act,
    *m_paste_rows_act,
    *m_drop_act,
    *m_separator1_act,
    *m_separator2_act;

  static void on_remove(const QModelIndex& parent, int start, int end, QModelIndex& index);
  void on_update();
  void connect_by(const std::vector<std::shared_ptr<task_connect::provider_allocator>>& allocators);

protected:
  void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;

private slots:
  void on_connect_mysql();
  void on_connect_odbc();
  void on_connect_oracle();
  void on_connect_osm();
  void on_connect_postgres();
  void on_open_file();
  void on_new_file();
  void on_copy_checked();
  void on_disconnect()  { m_mdl.disconnect(m_idx_menu); }
  void on_refresh();
  void on_refresh(provider_ptr pvd);
  void on_sql_console();
  void on_paste_layers();
  void on_zoom_to_fit();
  void on_snap_to_pixels();
  void on_use_projection();
  void on_copy();
  void on_paste_rows();
  void on_drop();
  void on_show_menu(QPoint point);
  void on_attributes();
  void emit_layers(std::vector<layer_ptr> lrs)  { emit signal_layers(lrs); }
  void emit_proj(brig::proj::shared_pj pj)  { emit signal_proj(pj); }
  void emit_rect(QRectF rect, brig::proj::shared_pj pj)  { emit signal_rect(rect, pj); }
  void emit_scale(double scale, brig::proj::shared_pj pj)  { emit signal_scale(scale, pj); }
  void emit_sql(provider_ptr pvd, std::vector<std::string> sqls)  { emit signal_sql(pvd, sqls); }
  void emit_disconnect(provider_ptr pvd)  { emit signal_disconnect(pvd); }

signals:
  void signal_layers(std::vector<layer_ptr> lrs);
  void signal_proj(brig::proj::shared_pj pj);
  void signal_rect(QRectF rect, brig::proj::shared_pj pj);
  void signal_scale(double scale, brig::proj::shared_pj pj);
  void signal_task(std::shared_ptr<task> tsk);
  void signal_sql(provider_ptr pvd, std::vector<std::string> sqls);
  void signal_disconnect(provider_ptr pvd);

public:
  explicit tree_view(QWidget* parent);
}; // tree_view

#endif // TREE_VIEW_H
