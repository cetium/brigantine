// Andrew Naplavkov

#include <algorithm>
#include <brig/database/sqlite/version.hpp>
#include <brig/proj/version.hpp>
#include <exception>
#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStringList>
#include <QTimer>
#include <QtGlobal>
#include "main_window.h"
#include "map_view.h"
#include "sql_view.h"
#include "task.h"
#include "tree_view.h"
#include "utilities.h"

main_window::main_window(QWidget* parent) : QMainWindow(parent)
{
  tree_view* tree(new tree_view);
  map_view* map(new map_view);
  sql_view* sql(new sql_view);

  m_tab = new QTabWidget;
  m_tab->setTabPosition(QTabWidget::East);
  m_map_tab = m_tab->addTab(map, QIcon(QPixmap(":/map.png").transformed(QTransform().rotate(-90))), "");
  m_sql_tab = m_tab->addTab(sql, QIcon(QPixmap(":/sql.png").transformed(QTransform().rotate(-90))), "");

  QSplitter* splitter(new QSplitter);
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(tree);
  splitter->addWidget(m_tab);

  QStatusBar* status_bar(new QStatusBar);
  status_bar->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(status_bar, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_stat_menu(const QPoint&)));

  m_pos_stat = new QLabel;
  m_pos_stat->setDisabled(true);
  m_pos_stat->setTextFormat(Qt::RichText);
  m_pos_stat->setText(rich_text(":/globe_disabled.png", ""));
  status_bar->addWidget(m_pos_stat);

  m_map_stat = new QLabel;
  m_map_stat->setDisabled(true);
  m_map_stat->setTextFormat(Qt::RichText);
  m_map_stat->setText(rich_text(":/map_disabled.png", ""));
  status_bar->addPermanentWidget(m_map_stat);

  m_sql_stat = new QLabel;
  m_sql_stat->setDisabled(true);
  m_sql_stat->setTextFormat(Qt::RichText);
  m_sql_stat->setText(rich_text(":/sql_disabled.png", ""));
  status_bar->addPermanentWidget(m_sql_stat);

  setCentralWidget(splitter);
  setStatusBar(status_bar);

  m_copy_map_stat = new QAction(QIcon(":/copy.png"), "copy to clipboard", this);
  m_copy_map_stat->setIconVisibleInMenu(true);
  connect(m_copy_map_stat, SIGNAL(triggered()), this, SLOT(copy_map_stat()));

  m_copy_sql_stat = new QAction(QIcon(":/copy.png"), "copy to clipboard", this);
  m_copy_sql_stat->setIconVisibleInMenu(true);
  connect(m_copy_sql_stat, SIGNAL(triggered()), this, SLOT(copy_sql_stat()));

  const float w(std::min<float>(width(), height()));
  resize(w, w / 4. * 3.);
  move(QApplication::desktop()->screen()->rect().center() - rect().center());

  QList<int> sizes;
  sizes.push_back(splitter->size().width() * .3);
  sizes.push_back(splitter->size().width() * .7);
  splitter->setSizes(sizes);

  qRegisterMetaType<connection_link>("connection_link");
  qRegisterMetaType<brig::proj::epsg>("brig::proj::epsg");
  qRegisterMetaType<std::shared_ptr<task>>("std::shared_ptr<task>");
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  qRegisterMetaType<std::vector<layer_link>>("std::vector<layer_link>");
  connect(tree, SIGNAL(signal_layers(std::vector<layer_link>)), map, SLOT(set_layers(std::vector<layer_link>)));
  connect(tree, SIGNAL(signal_view(QRectF, brig::proj::epsg)), map, SLOT(set_view(QRectF, brig::proj::epsg)));
  connect(tree, SIGNAL(signal_proj(brig::proj::epsg)), map, SLOT(set_proj(brig::proj::epsg)));
  connect(tree, SIGNAL(signal_attributes(layer_link)), map, SLOT(on_attributes(layer_link)));
  connect
    ( tree, SIGNAL(signal_commands(connection_link, std::vector<std::string>))
    , sql, SLOT(on_commands(connection_link, std::vector<std::string>))
    );
  connect(tree, SIGNAL(signal_disconnect(connection_link)), sql, SLOT(on_disconnect(connection_link)));
  connect(tree, SIGNAL(signal_task(std::shared_ptr<task>)), sql, SLOT(push(std::shared_ptr<task>)));
  connect(sql, SIGNAL(signal_start()), this, SLOT(on_sql_start()));
  connect(sql, SIGNAL(signal_process(QString)), this, SLOT(on_sql_process(QString)));
  connect(sql, SIGNAL(signal_idle()), this, SLOT(on_sql_idle()));
  connect(sql, SIGNAL(signal_commands()), this, SLOT(on_sql_commands()));
  connect(map, SIGNAL(signal_start()), this, SLOT(on_map_start()));
  connect(map, SIGNAL(signal_process(QString)), this, SLOT(on_map_process(QString)));
  connect(map, SIGNAL(signal_idle()), this, SLOT(on_map_idle()));
  connect(map, SIGNAL(signal_coords(QString)), this, SLOT(on_map_coords(QString)));
  connect(map, SIGNAL(signal_scene(brig::proj::epsg)), this, SLOT(on_map_scene(brig::proj::epsg)));
  connect
    ( map, SIGNAL(signal_commands(connection_link, std::vector<std::string>))
    , sql, SLOT(on_commands(connection_link, std::vector<std::string>))
    );
  connect(map, SIGNAL(signal_task(std::shared_ptr<task>)), sql, SLOT(push(std::shared_ptr<task>)));

  setWindowIcon(QIcon(":/wheel.png"));
  try  { on_map_scene(latlon()); }
  catch (const std::exception&)  {}

  QTimer* timer(new QTimer(this));
  timer->start(100);
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
}

void main_window::on_map_scene(brig::proj::epsg pj)
{
  m_tab->setCurrentIndex(m_map_tab);
  setWindowTitle("brigantine , " + QString().setNum(int(pj)));
}

void main_window::on_map_coords(const QString& msg)
{
  m_pos_stat->setDisabled(msg.isEmpty());
  m_pos_stat->setText(rich_text(msg.isEmpty()? ":/globe_disabled.png": ":/globe.png", msg));
}

QString status(const QTime& time, QString msg)
{
  static const int MessageLimit = 30;
  QString res;
  res.setNum((double)time.elapsed() / 1000., 'f', 1);
  if (msg.size() > MessageLimit) msg = msg.left(MessageLimit) + "...";
  if (!msg.isEmpty()) res += " , " + msg;
  return res;
}

void main_window::on_map_start()
{
  m_map_msg = "0";
  m_map_time.restart();
  m_map_stat->setEnabled(true);
  m_map_stat->setText(rich_text(":/map.png", status(m_map_time, m_map_msg)));
  m_map_stat->setToolTip("");
}

void main_window::on_map_idle()
{
  m_map_stat->setDisabled(true);
  m_map_stat->setText(rich_text(":/map_disabled.png", status(m_map_time, m_map_msg)));
  m_map_stat->setToolTip(m_map_msg);
}

void main_window::on_sql_start()
{
  m_sql_msg = "0";
  m_sql_time.restart();
  m_sql_stat->setEnabled(true);
  m_sql_stat->setText(rich_text(":/sql.png", status(m_sql_time, m_sql_msg)));
  m_sql_stat->setToolTip("");
}

void main_window::on_sql_idle()
{
  m_sql_stat->setDisabled(true);
  m_sql_stat->setText(rich_text(":/sql_disabled.png", status(m_sql_time, m_sql_msg)));
  m_sql_stat->setToolTip(m_sql_msg);
}

void main_window::on_timer()
{
  if (m_map_stat->isEnabled()) m_map_stat->setText(rich_text(":/map.png", status(m_map_time, m_map_msg)));
  if (m_sql_stat->isEnabled()) m_sql_stat->setText(rich_text(":/sql.png", status(m_sql_time, m_sql_msg)));
}

void main_window::show_stat_menu(const QPoint& pnt)
{
  QStatusBar* status_bar(statusBar());
  QWidget* widget(status_bar->childAt(pnt));
  QList<QAction*> actions;
  if (widget == m_map_stat)
    actions.append(m_copy_map_stat);
  else if (widget == m_sql_stat)
    actions.append(m_copy_sql_stat);
  if (!actions.empty())
    QMenu::exec(actions, status_bar->mapToGlobal(pnt));
}

void main_window::copy_map_stat()
{
  QApplication::clipboard()->setText(m_map_msg);
}

void main_window::copy_sql_stat()
{
  QApplication::clipboard()->setText(m_sql_msg);
}

void main_window::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F1)
  {
    QStringList props;
    props.append(QString("brigantine: ") + __DATE__ + ", " + to_string(sizeof(void*) * 8) + "-bit");
    props.append(QString());
    props.append(QString("qt: ") + qVersion());
    if (!brig::proj::version().empty()) props.append(QString::fromStdString("proj: " + brig::proj::version()));
    if (!brig::database::sqlite::sqlite3_libversion().empty()) props.append(QString::fromStdString("sqlite: " + brig::database::sqlite::sqlite3_libversion()));
    if (!brig::database::sqlite::spatialite_version().empty()) props.append(QString::fromStdString("spatialite: " + brig::database::sqlite::spatialite_version()));
    props.append(QString());
    props.append("andrew.naplavkov@gmail.com");
    QMessageBox::about(this, "about", props.join("\n"));
  }
  else
    QWidget::keyPressEvent(event);
}
