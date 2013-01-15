// Andrew Naplavkov

#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include <QAction>
#include <QModelIndex>
#include <QPoint>
#include <QString>
#include <QTreeView>
#include <QWidget>
#include <vector>
#include "task.h"
#include "tree_model.h"

class tree_view : public QTreeView {
  Q_OBJECT

  tree_model m_mdl;
  QModelIndex m_idx_menu;
  std::vector<layer_link> m_lrs_copy;

  QAction
    *m_connect_mysql_act,
    *m_connect_odbc_act,
    *m_connect_oracle_act,
    *m_connect_osm_act,
    *m_connect_postgres_act,
    *m_open_file_act,
    *m_new_file_act,
    *m_copy_rendered_layers_act,
    *m_refresh_act,
    *m_sql_console_act,
    *m_paste_layers_act,
    *m_disconnect_act,
    *m_zoom_to_fit_act,
    *m_use_projection_act,
    *m_attributes_act,
    *m_copy_act,
    *m_paste_rows_act,
    *m_drop_act,
    *m_separator1_act,
    *m_separator2_act;

  static void on_remove(const QModelIndex& parent, int start, int end, QModelIndex& index);
  void on_update();

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
  void on_copy_rendered_layers();
  void on_disconnect()  { m_mdl.disconnect(m_idx_menu); }
  void on_refresh()  { m_mdl.refresh(m_idx_menu); }
  void on_sql_console()  { m_mdl.sql_console(m_idx_menu); }
  void on_paste_layers()  { m_mdl.paste_layers(m_lrs_copy, m_idx_menu); }
  void on_zoom_to_fit()  { m_mdl.zoom_to_fit(m_idx_menu); }
  void on_use_projection()  { m_mdl.use_projection(m_idx_menu); }
  void on_copy();
  void on_paste_rows()  { m_mdl.paste_rows(m_lrs_copy.front(), m_idx_menu); }
  void on_drop()  { m_mdl.drop(m_idx_menu); }
  void on_show_menu(QPoint point);
  void emit_attributes()  { emit signal_attributes(m_mdl.get_layer(m_idx_menu)); }
  void emit_commands(connection_link dbc, std::vector<std::string> sqls)  { emit signal_commands(dbc, sqls); }
  void emit_disconnect(connection_link dbc)  { emit signal_disconnect(dbc); }
  void emit_layers(std::vector<layer_link> lim)  { emit signal_layers(lim); }
  void emit_proj(brig::proj::shared_pj pj)  { emit signal_proj(pj); }
  void emit_task(std::shared_ptr<task> tsk)  { emit signal_task(tsk); }
  void emit_view(QRectF rect, brig::proj::shared_pj pj)  { emit signal_view(rect, pj); }

signals:
  void signal_layers(std::vector<layer_link> lrs);
  void signal_view(QRectF rect, brig::proj::shared_pj pj);
  void signal_proj(brig::proj::shared_pj pj);
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
  void signal_disconnect(connection_link dbc);
  void signal_task(std::shared_ptr<task> tsk);
  void signal_attributes(layer_link lr);

public:
  explicit tree_view(QWidget* parent);
}; // tree_view

#endif // TREE_VIEW_H
