// Andrew Naplavkov

#include <algorithm>
#include <brig/database/sqlite/command_allocator.hpp>
#include <exception>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QList>
#include <QMenu>
#include <QString>
#include <QStringList>
#include "connection.h"
#include "dialog_connect.h"
#include "dialog_odbc.h"
#include "dialog_shape.h"
#include "layer.h"
#include "layer_geometry.h"
#include "tree_view.h"
#include "utilities.h"

tree_view::tree_view(QWidget* parent) : QTreeView(parent)
{
  setModel(&m_mdl);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setHeaderHidden(true);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_menu(const QPoint&)));

  m_connect_db2 = new QAction(QIcon(":/res/db2.png"), "connect to DB2", this);
  m_connect_db2->setIconVisibleInMenu(true);
  connect(m_connect_db2, SIGNAL(triggered()), this, SLOT(connect_db2()));

  m_connect_mysql = new QAction(QIcon(":/res/mysql.png"), "connect to MySQL", this);
  m_connect_mysql->setIconVisibleInMenu(true);
  connect(m_connect_mysql, SIGNAL(triggered()), this, SLOT(connect_mysql()));

  m_connect_odbc = new QAction(QIcon(":/res/odbc.png"), "connect using ODBC", this);
  m_connect_odbc->setIconVisibleInMenu(true);
  connect(m_connect_odbc, SIGNAL(triggered()), this, SLOT(connect_odbc()));

  m_connect_oracle = new QAction(QIcon(":/res/oracle.png"), "connect to Oracle", this);
  m_connect_oracle->setIconVisibleInMenu(true);
  connect(m_connect_oracle, SIGNAL(triggered()), this, SLOT(connect_oracle()));

  m_connect_postgres = new QAction(QIcon(":/res/postgres.png"), "connect to Postgres", this);
  m_connect_postgres->setIconVisibleInMenu(true);
  connect(m_connect_postgres, SIGNAL(triggered()), this, SLOT(connect_postgres()));

  m_open_sqlite = new QAction(QIcon(":/res/sqlite.png"), "open SQLite file", this);
  m_open_sqlite->setIconVisibleInMenu(true);
  connect(m_open_sqlite, SIGNAL(triggered()), this, SLOT(open_sqlite()));

  m_new_sqlite = new QAction(QIcon(":/res/add_folder.png"), "new SQLite file", this);
  m_new_sqlite->setIconVisibleInMenu(true);
  connect(m_new_sqlite, SIGNAL(triggered()), this, SLOT(new_sqlite()));

  m_copy_shp = new QAction(QIcon(":/res/shp.png"), "copy shapefile", this);
  m_copy_shp->setIconVisibleInMenu(true);
  connect(m_copy_shp, SIGNAL(triggered()), this, SLOT(copy_shp()));

  m_refresh = new QAction(QIcon(":/res/refresh.png"), "refresh", this);
  m_refresh->setIconVisibleInMenu(true);
  connect(m_refresh, SIGNAL(triggered()), this, SLOT(refresh()));

  m_use_in_sql = new QAction(QIcon(":/res/sql.png"), "use in SQL", this);
  m_use_in_sql->setIconVisibleInMenu(true);
  connect(m_use_in_sql, SIGNAL(triggered()), this, SLOT(use_in_sql()));

  m_paste_layer = new QAction(QIcon(":/res/paste.png"), "paste layer", this);
  m_paste_layer->setIconVisibleInMenu(true);
  connect(m_paste_layer, SIGNAL(triggered()), this, SLOT(paste_layer()));

  m_disconnect = new QAction(QIcon(":/res/disconnect.png"), "disconnect", this);
  m_disconnect->setIconVisibleInMenu(true);
  connect(m_disconnect, SIGNAL(triggered()), this, SLOT(disconnect()));

  m_zoom_to_fit = new QAction(QIcon(":/res/zoom.png"), "zoom to fit", this);
  m_zoom_to_fit->setIconVisibleInMenu(true);
  connect(m_zoom_to_fit, SIGNAL(triggered()), this, SLOT(zoom_to_fit()));

  m_use_projection = new QAction(QIcon(":/res/map.png"), "use the projection", this);
  m_use_projection->setIconVisibleInMenu(true);
  connect(m_use_projection, SIGNAL(triggered()), this, SLOT(use_projection()));

  m_attributes = new QAction(QIcon(":/res/sql.png"), "attributes", this);
  m_attributes->setIconVisibleInMenu(true);
  connect(m_attributes, SIGNAL(triggered()), this, SLOT(attributes()));

  m_copy = new QAction(QIcon(":/res/copy.png"), "copy", this);
  m_copy->setIconVisibleInMenu(true);
  connect(m_copy, SIGNAL(triggered()), this, SLOT(copy()));

  m_paste_rows = new QAction(QIcon(":/res/paste.png"), "paste rows", this);
  m_paste_rows->setIconVisibleInMenu(true);
  connect(m_paste_rows, SIGNAL(triggered()), this, SLOT(paste_rows()));

  m_drop = new QAction(QIcon(":/res/delete.png"), "drop", this);
  m_drop->setIconVisibleInMenu(true);
  connect(m_drop, SIGNAL(triggered()), this, SLOT(drop()));

  m_separator = new QAction(QIcon(""), "", this);
  m_separator->setSeparator(true);

  qRegisterMetaType<connection_link>("connection_link");
  qRegisterMetaType<brig::proj::epsg>("brig::proj::epsg");
  qRegisterMetaType<std::shared_ptr<task>>("std::shared_ptr<task>");
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  qRegisterMetaType<std::vector<layer_link>>("std::vector<layer_link>");
  connect(&m_mdl, SIGNAL(signal_layers(std::vector<layer_link>)), this, SLOT(on_layers(std::vector<layer_link>)));
  connect(&m_mdl, SIGNAL(signal_view(QRectF, brig::proj::epsg)), this, SLOT(on_view(QRectF, brig::proj::epsg)));
  connect(&m_mdl, SIGNAL(signal_proj(brig::proj::epsg)), this, SLOT(on_proj(brig::proj::epsg)));
  connect
    ( &m_mdl, SIGNAL(signal_commands(connection_link, std::vector<std::string>))
    , this, SLOT(on_commands(connection_link, std::vector<std::string>))
    );
  connect(&m_mdl, SIGNAL(signal_disconnect(connection_link)), this, SLOT(on_disconnect(connection_link)));
  connect(&m_mdl, SIGNAL(signal_task(std::shared_ptr<task>)), this, SLOT(on_task(std::shared_ptr<task>)));
}

void tree_view::connect_db2()
{
  try
  {
    static dialog_connect dlg(":/res/db2.png", "192.168.1.152", 50000, "TEST", "DB2INST1");
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_odbc("DRIVER=" + QString().fromUtf8(get_ibm_odbc_driver().c_str()) + ";HOSTNAME=" + dlg.host() + ";PORT=" + to_string(dlg.port()) + ";DATABASE=" + dlg.db() + ";UID=" + dlg.usr() + ";PWD=" + dlg.pwd() + ";");
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::connect_mysql()
{
  try
  {
    static dialog_connect dlg(":/res/mysql.png", "192.168.1.56", 3306, "test", "root");
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_mysql(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::connect_odbc()
{
  try
  {
    static dialog_odbc dlg;
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_odbc(dlg.str());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::connect_oracle()
{
  try
  {
    static dialog_connect dlg(":/res/oracle.png", "192.168.1.152", 1521, "XE", "SYSTEM");
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_oracle(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::connect_postgres()
{
  try
  {
    static dialog_connect dlg(":/res/postgres.png", "gis-lab.info", 5432, "osm_shp", "guest");
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_postgres(dlg.host(), dlg.port(), dlg.db(), dlg.usr(), dlg.pwd());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::open_sqlite()
{
  try
  {
    static QFileDialog dlg(this, "open SQLite files", QDir::currentPath(), "SQLite files (*.sqlite);;All files (*.*)");
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    QStringList files = dlg.selectedFiles();
    for (int i(0); i < files.size(); ++i)
      m_mdl.connect_sqlite(files[i], false);
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::new_sqlite()
{
  try
  {
    static QFileDialog dlg(this, "new SQLite file", QDir::currentPath(), "SQLite files (*.sqlite)");
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    if (!dlg.exec()) return;
    wait_cursor w;
    QFileInfo file(dlg.selectedFiles().value(0));
    if (file.exists() && !QFile::remove(file.filePath()))
    {
      show_message("file removing error");
      return;
    }
    if (file.suffix().isEmpty()) file = QFileInfo(file.dir(), file.fileName() + ".sqlite");
    m_mdl.connect_sqlite(file.filePath(), true);
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::copy_shp()
{
  try
  {
    m_idx_copy = QModelIndex();

    static dialog_shape dlg;
    if (dlg.exec() != QDialog::Accepted) return;

    wait_cursor w;
    QFileInfo file(dlg.selectedFiles().value(0));
    std::string base(file.baseName().toUtf8().constData());
    std::string path(QFileInfo(file.dir(), file.baseName()).filePath().toUtf8().constData());
    std::string charset(dlg.charset().toUtf8().constData());
    std::string epsg(dlg.epsg().toUtf8().constData());

    auto allocator(std::make_shared<brig::database::sqlite::command_allocator>(":memory:"));
    connection_link dbc(new connection(allocator, ":memory:"));
    dbc->get_command()->exec("CREATE VIRTUAL TABLE \"" + base + "\" USING VirtualShape('" + path + "', '" + charset + "', " + epsg + ")");

    brig::database::identifier id; id.name = base;
    auto tbl(dbc->get_table_definition(id));
    auto col_key(std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "PK_UID" == c.name; }));
    if (col_key == std::end(tbl.columns)) col_key = std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "PKUID" == c.name; });
    auto col_geo(std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "Geometry" == c.name; }));
    if (col_key == std::end(tbl.columns) || col_geo == std::end(tbl.columns)) return;

    col_geo->type = brig::database::Geometry;
    col_geo->dbms_type.name = "GEOMETRY";
    col_geo->dbms_type_lcase.name = "geometry";
    col_geo->srid = dlg.epsg().toInt();
    col_geo->epsg = dlg.epsg().toInt();
    col_geo->query_expression = "AsBinary(\"GEOMETRY\")";

    brig::database::index_definition pk;
    pk.type = brig::database::Primary;
    pk.columns.push_back(col_key->name);
    tbl.indexes.push_back(pk);

    brig::database::index_definition idx;
    idx.type = brig::database::Spatial;
    idx.columns.push_back(col_geo->name);
    tbl.indexes.push_back(idx);

    m_lr_copy = layer_link(new layer_geometry(dbc, id, tbl));
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::copy()
{
  m_idx_copy = m_idx_menu;
  m_lr_copy = m_mdl.get_layer(m_idx_copy);
}

void tree_view::on_update()
{
  m_drop->setEnabled(m_mdl.is_layer(m_idx_menu) && m_mdl.get_layer(m_idx_menu)->is_writable());
  m_paste_rows->setEnabled(m_drop->isEnabled() && m_lr_copy && m_mdl.get_layer(m_idx_menu)->get_levels() == m_lr_copy->get_levels());
  m_paste_layer->setEnabled(m_mdl.is_connection(m_idx_menu) && m_lr_copy);
}

void tree_view::show_menu(const QPoint& pnt)
{
  m_idx_menu = indexAt(pnt);
  on_update();

  QList<QAction*> actions;
  if (m_mdl.is_connection(m_idx_menu))
  {
    actions.append(m_refresh);
    actions.append(m_use_in_sql);
    actions.append(m_paste_layer);
    actions.append(m_disconnect);
    actions.append(m_separator);
  }
  else if (m_mdl.is_layer(m_idx_menu))
  {
    actions.append(m_zoom_to_fit);
    actions.append(m_use_projection);
    actions.append(m_attributes);
    actions.append(m_copy);
    actions.append(m_paste_rows);
    actions.append(m_drop);
    actions.append(m_separator);
  }
  if (!get_ibm_odbc_driver().empty()) actions.append(m_connect_db2);
  actions.append(m_connect_mysql);
  actions.append(m_connect_odbc);
  actions.append(m_connect_oracle);
  actions.append(m_connect_postgres);
  actions.append(m_open_sqlite);
  actions.append(m_new_sqlite);
  actions.append(m_copy_shp);
  QMenu::exec(actions, mapToGlobal(pnt));
}

void tree_view::on_remove(const QModelIndex& parent, int start, int end, QModelIndex& index)
{
  if (!index.isValid())
    return;
  else if (!parent.isValid())
  {
    const int parent_row(static_cast<tree_item*>(index.internalPointer())->m_parent->position());
    if (parent_row >= start && parent_row <= end) index = QModelIndex();
  }
  else if (index.parent() == parent)
  {
    if (index.row() > end)
      index = index.model()->index(index.row() - (end - start + 1), index.column(), index.parent());
    else if (index.row() >= start)
      index = QModelIndex();
  }
}

void tree_view::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
  on_remove(parent, start, end, m_idx_menu);
  const bool valid(m_idx_copy.isValid());
  on_remove(parent, start, end, m_idx_copy);
  if (valid && !m_idx_copy.isValid()) m_lr_copy = layer_link();
  on_update();
  QTreeView::rowsAboutToBeRemoved(parent, start, end);
}
