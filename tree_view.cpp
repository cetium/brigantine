// Andrew Naplavkov

#include <algorithm>
#include <brig/database/sqlite/command_allocator.hpp>
#include <exception>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMenu>
#include <QRegExpValidator>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include "connection.h"
#include "dialog_oci.h"
#include "dialog_odbc.h"
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

  m_connect_oci = new QAction(QIcon(":/res/oracle.png"), "connect to Oracle", this);
  m_connect_oci->setIconVisibleInMenu(true);
  connect(m_connect_oci, SIGNAL(triggered()), this, SLOT(connect_oci()));

  m_connect_odbc = new QAction(QIcon(":/res/odbc.png"), "connect using ODBC", this);
  m_connect_odbc->setIconVisibleInMenu(true);
  connect(m_connect_odbc, SIGNAL(triggered()), this, SLOT(connect_odbc()));

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

void tree_view::connect_oci()
{
  try
  {
    dialog_oci dlg;
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_oci(dlg.srv(), dlg.usr(), dlg.pwd());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::connect_odbc()
{
  try
  {
    dialog_odbc dlg;
    if (dlg.exec() != QDialog::Accepted) return;
    wait_cursor w;
    m_mdl.connect_odbc(dlg.str());
  }
  catch (const std::exception& e)  { show_message(e.what()); }
}

void tree_view::open_sqlite()
{
  try
  {
    QFileDialog dlg(this, "open SQLite file", QDir::currentPath(), "SQLite files (*.sqlite);;All files (*.*)");
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
    QFileDialog dlg(this, "new SQLite file", QDir::currentPath(), "SQLite files (*.sqlite)");
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
  struct pair { const char *first, *second; };
  static const pair Charsets[] = {
  { "ARMSCII-8", "Armenian" },
  { "ASCII", "US-ASCII" },
  { "BIG5", "Chinese/Traditional" },
  { "BIG5-HKSCS", "Chinese/Hong Kong" },
  { "BIG5-HKSCS:1999", "Chinese/Hong Kong" },
  { "BIG5-HKSCS:2001", "Chinese/Hong Kong" },
  { "CP850", "DOS/OEM Western Europe" },
  { "CP862", "DOS/OEM Hebrew" },
  { "CP866", "DOS/OEM Cyrillic" },
  { "CP874", "DOS/OEM Thai" },
  { "CP932", "DOS/OEM Japanese" },
  { "CP936", "DOS/OEM Chinese" },
  { "CP949", "DOS/OEM Korean" },
  { "CP950", "DOS/OEM Chinese/Big5" },
  { "CP1133", "Laotian" },
  { "CP1250", "Windows Central Europe" },
  { "CP1251", "Windows Cyrillic" },
  { "CP1252", "Windows Latin 1" },
  { "CP1253", "Windows Greek" },
  { "CP1254", "Windows Turkish" },
  { "CP1255", "Windows Hebrew" },
  { "CP1256", "Windows Arabic" },
  { "CP1257", "Windows Baltic" },
  { "CP1258", "Windows Vietnamese" },
  { "EUC-CN", "Chinese" },
  { "EUC-JP", "Japanese" },
  { "EUC-KR", "Korean" },
  { "EUC-TW", "Taiwan" },
  { "GB18030", "Chinese/National Standard" },
  { "GBK", "Chinese/Simplified" },
  { "Georgian-Academy", "Georgian" },
  { "Georgian-PS", "Georgian" },
  { "HZ", "Chinese" },
  { "ISO-2022-CN", "Chinese" },
  { "ISO-2022-CN-EXT", "Chinese" },
  { "ISO-2022-JP", "Japanese" },
  { "ISO-2022-JP-1", "Japanese" },
  { "ISO-2022-JP-2", "Japanese" },
  { "ISO-2022-KR", "Korean" },
  { "ISO-8859-1", "Latin-1 Western European" },
  { "ISO-8859-2", "Latin-2 Central European" },
  { "ISO-8859-3", "Latin-3 South European" },
  { "ISO-8859-4", "Latin-4 North European" },
  { "ISO-8859-5", "Latin/Cyrillic" },
  { "ISO-8859-6", "Latin/Arabic" },
  { "ISO-8859-7", "Latin/Greek" },
  { "ISO-8859-8", "Latin/Hebrew" },
  { "ISO-8859-9", "Latin-5 Turkish" },
  { "ISO-8859-10", "Latin-6 Nordic" },
  { "ISO-8859-11", "Latin/Thai" },
  { "ISO-8859-13", "Latin-7 Baltic Rim" },
  { "ISO-8859-14", "Latin-8 Celtic" },
  { "ISO-8859-15", "Latin-9" },
  { "ISO-8859-16", "Latin-10 South-Eastern European" },
  { "JOHAB", "Korean" },
  { "KOI8-R", "Russian" },
  { "KOI8-U", "Ukrainian" },
  { "KOI8-RU", "Belarusian" },
  { "KOI8-T", "Tajik" },
  { "MacArabic", "MAC Arabic" },
  { "MacCentralEurope", "MAC Central Europe" },
  { "MacCroatian", "MAC Croatian" },
  { "MacCyrillic", "MAC Cyrillic" },
  { "MacGreek", "MAC Greek" },
  { "MacHebrew", "MAC Hebrew" },
  { "MacIceland", "MAC Iceland" },
  { "Macintosh", "MAC" },
  { "MacRoman", "MAC European/Western languages" },
  { "MacRomania", "MAC Romania" },
  { "MacThai", "MAC Thai" },
  { "MacTurkish", "MAC Turkish" },
  { "MacUkraine", "MAC Ukraine" },
  { "MuleLao-1", "Laotian" },
  { "PT154", "Kazakh" },
  { "RK1048", "Kazakh" },
  { "SHIFT_JIS", "Japanese" },
  { "TCVN", "Vietnamese" },
  { "TIS-620", "Thai" },
  { "UTF-8", "UNICODE/Universal" },
  { "VISCII", "Vietnamese" }
  }; // Charsets

  try
  {
    m_idx_copy = QModelIndex();

    QFileDialog dlg(this, "copy shapefile", QDir::currentPath(), "shapefiles (*.shp)");
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QLabel* charset_lbl = new QLabel("Charset:", &dlg);
    QComboBox* charset_combo = new QComboBox(&dlg);
    for (int i(0), size(sizeof(Charsets) / sizeof(pair)); i < size; ++i)
    {
      QString itm;
      itm += Charsets[i].first;
      itm += ", ";
      itm += Charsets[i].second;
      charset_combo->addItem(itm, Charsets[i].first);
    }
    int pos(-1);
  #ifdef Q_OS_WIN32
    pos = charset_combo->findData("CP1252");
  #elif Q_OS_MAC
    pos = charset_combo->findData("MacCentralEurope");
  #else
    pos = charset_combo->findData("UTF-8");
  #endif
    if (pos >= 0) charset_combo->setCurrentIndex(pos);

    QLabel* epsg_lbl = new QLabel("EPSG:", &dlg);
    QLineEdit* epsg_edit = new QLineEdit("4326", &dlg);
    QRegExpValidator vlr(QRegExp("[1-9]\\d{0,4}"), 0);
    epsg_edit->setValidator(&vlr);

    QGridLayout* layout = (QGridLayout*)dlg.layout();
    const int rows = layout->rowCount();
    layout->addWidget(charset_lbl, rows, 0);
    layout->addWidget(charset_combo, rows, 1);
    layout->addWidget(epsg_lbl, rows + 1, 0);
    layout->addWidget(epsg_edit, rows + 1, 1);
    if (dlg.exec() != QDialog::Accepted) return;

    wait_cursor w;
    QFileInfo file(dlg.selectedFiles().value(0));
    std::string base(file.baseName().toUtf8().constData());
    std::string path(QFileInfo(file.dir(), file.baseName()).filePath().toUtf8().constData());
    std::string charset(qvariant_cast<QString>(charset_combo->itemData(charset_combo->currentIndex())).toUtf8().constData());

    auto allocator(std::make_shared<brig::database::sqlite::command_allocator>(":memory:"));
    connection_link dbc(new connection(allocator, ":memory:"));
    dbc->get_command()->exec("CREATE VIRTUAL TABLE \"" + base + "\" USING VirtualShape('" + path + "', '" + charset + "', " + std::string(epsg_edit->text().toUtf8().constData()) + ")");

    brig::database::identifier id; id.name = base;
    auto tbl(dbc->get_table_definition(id));
    auto col_key(std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "PK_UID" == c.name; }));
    if (col_key == std::end(tbl.columns)) col_key = std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "PKUID" == c.name; });
    auto col_geo(std::find_if(std::begin(tbl.columns), std::end(tbl.columns), [](const brig::database::column_definition& c){ return "Geometry" == c.name; }));
    if (col_key == std::end(tbl.columns) || col_geo == std::end(tbl.columns)) return;

    col_geo->type = brig::database::Geometry;
    col_geo->dbms_type.name = "GEOMETRY";
    col_geo->lower_case_type.name = "geometry";
    col_geo->srid = epsg_edit->text().toInt();
    col_geo->epsg = epsg_edit->text().toInt();

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
  actions.append(m_connect_oci);
  actions.append(m_connect_odbc);
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
