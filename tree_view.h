// Andrew Naplavkov

#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include <QAction>
#include <QFileDialog>
#include <QModelIndex>
#include <QPoint>
#include <QString>
#include <QTreeView>
#include <QWidget>
#include <vector>
#include "dialog_odbc.h"
#include "dialog_shape.h"
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
    *m_connect_postgres_act,
    *m_open_sqlite_act,
    *m_new_sqlite_act,
    *m_copy_shp_act,
    *m_copy_rendered_layers_act,
    *m_refresh_act,
    *m_use_in_sql_act,
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

  dialog_odbc* m_connect_odbc_dlg;
  dialog_shape* m_copy_shp_dlg;

  QFileDialog
    *m_open_sqlite_dlg,
    *m_new_sqlite_dlg;

  static void on_remove(const QModelIndex& parent, int start, int end, QModelIndex& index);
  void on_update();

protected:
  virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

private slots:
  void connect_mysql();
  void connect_odbc();
  void connect_oracle();
  void connect_postgres();
  void open_sqlite();
  void new_sqlite();
  void copy_shp();
  void copy_rendered_layers();
  void disconnect()  { m_mdl.disconnect(m_idx_menu); }
  void refresh()  { m_mdl.refresh(m_idx_menu); }
  void use_in_sql()  { m_mdl.use_in_sql(m_idx_menu); }
  void paste_layers()  { m_mdl.paste_layers(m_lrs_copy, m_idx_menu); }
  void zoom_to_fit()  { m_mdl.zoom_to_fit(m_idx_menu); }
  void use_projection()  { m_mdl.use_projection(m_idx_menu); }
  void attributes()  { emit signal_attributes(m_mdl.get_layer(m_idx_menu)); }
  void copy();
  void paste_rows()  { m_mdl.paste_rows(m_lrs_copy.front(), m_idx_menu); }
  void drop()  { m_mdl.drop(m_idx_menu); }
  void show_menu(const QPoint& pnt);
  void on_layers(std::vector<layer_link> lim)  { emit signal_layers(lim); }
  void on_view(const QRectF& rect, const brig::proj::epsg& pj)  { emit signal_view(rect, pj); }
  void on_proj(const brig::proj::epsg& pj)  { emit signal_proj(pj); }
  void on_commands(connection_link dbc, std::vector<std::string> sqls)  { emit signal_commands(dbc, sqls); }
  void on_disconnect(connection_link dbc)  { emit signal_disconnect(dbc); }
  void on_task(std::shared_ptr<task> tsk)  { emit signal_task(tsk); }

signals:
  void signal_layers(std::vector<layer_link> lrs);
  void signal_view(const QRectF& rect, const brig::proj::epsg& pj);
  void signal_proj(const brig::proj::epsg& pj);
  void signal_commands(connection_link dbc, std::vector<std::string> sqls);
  void signal_disconnect(connection_link dbc);
  void signal_task(std::shared_ptr<task> tsk);
  void signal_attributes(layer_link lr);

public:
  explicit tree_view(QWidget* parent);
}; // tree_view

#endif // TREE_VIEW_H
