// Andrew Naplavkov

#include <algorithm>
#include <boost/version.hpp>
#include <brig/osm/version.hpp>
#include <brig/database/mysql/client_version.hpp>
#include <brig/database/odbc/drivers.hpp>
#include <brig/database/oracle/client_version.hpp>
#include <brig/database/postgres/client_version.hpp>
#include <brig/database/sqlite/version.hpp>
#include <brig/gdal/version.hpp>
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
#include <QtGlobal>
#include "main_window.h"
#include "map_view.h"
#include "sql_view.h"
#include "task.h"
#include "tree_view.h"
#include "utilities.h"

main_window::main_window()
{
  tree_view* tree(new tree_view(0));
  map_view* map(new map_view(0));
  sql_view* sql(new sql_view(0));

  m_tab = new QTabWidget;
  m_tab->setTabPosition(QTabWidget::East);
  m_map_tab = m_tab->addTab(map, QIcon(QPixmap(":/res/map.png").transformed(QTransform().rotate(-90))), "");
  m_sql_tab = m_tab->addTab(sql, QIcon(QPixmap(":/res/sql.png").transformed(QTransform().rotate(-90))), "");

  QSplitter* splitter(new QSplitter);
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(tree);
  splitter->addWidget(m_tab);

  QStatusBar* status_bar(new QStatusBar);
  status_bar->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(status_bar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_show_stat_menu(QPoint)));

  m_proj_stat = new QLabel;
  m_proj_stat->setDisabled(true);
  m_proj_stat->setTextFormat(Qt::RichText);
  m_proj_stat->setText(rich_text(":/res/map_disabled.png", "", false));
  status_bar->addWidget(m_proj_stat);

  m_pos_stat = new QLabel;
  m_pos_stat->setDisabled(true);
  m_pos_stat->setTextFormat(Qt::RichText);
  m_pos_stat->setText(rich_text(":/res/globe_disabled.png", "", false));
  status_bar->addWidget(m_pos_stat);

  m_map_stat = new QLabel;
  m_map_stat->setDisabled(true);
  m_map_stat->setTextFormat(Qt::RichText);
  m_map_stat->setText(rich_text(":/res/map_disabled.png", "", true));
  status_bar->addPermanentWidget(m_map_stat);

  m_sql_stat = new QLabel;
  m_sql_stat->setDisabled(true);
  m_sql_stat->setTextFormat(Qt::RichText);
  m_sql_stat->setText(rich_text(":/res/sql_disabled.png", "", true));
  status_bar->addPermanentWidget(m_sql_stat);

  setCentralWidget(splitter);
  setStatusBar(status_bar);

  m_copy_proj_stat = new QAction(QIcon(":/res/copy.png"), "copy to clipboard", this);
  m_copy_proj_stat->setIconVisibleInMenu(true);
  connect(m_copy_proj_stat, SIGNAL(triggered()), this, SLOT(on_copy_proj_stat()));

  m_copy_map_stat = new QAction(QIcon(":/res/copy.png"), "copy to clipboard", this);
  m_copy_map_stat->setIconVisibleInMenu(true);
  connect(m_copy_map_stat, SIGNAL(triggered()), this, SLOT(on_copy_map_stat()));

  m_copy_sql_stat = new QAction(QIcon(":/res/copy.png"), "copy to clipboard", this);
  m_copy_sql_stat->setIconVisibleInMenu(true);
  connect(m_copy_sql_stat, SIGNAL(triggered()), this, SLOT(on_copy_sql_stat()));

  const float w(std::min<float>(width(), height()));
  resize(w, w / 4. * 3.);
  move(QApplication::desktop()->screen()->rect().center() - rect().center());

  QList<int> sizes;
  sizes.push_back(splitter->size().width() * .3);
  sizes.push_back(splitter->size().width() * .7);
  splitter->setSizes(sizes);

  qRegisterMetaType<provider_ptr>("provider_ptr");
  qRegisterMetaType<brig::proj::shared_pj>("brig::proj::shared_pj");
  qRegisterMetaType<std::shared_ptr<task>>("std::shared_ptr<task>");
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  qRegisterMetaType<std::vector<layer_ptr>>("std::vector<layer_ptr>");

  connect(tree, SIGNAL(signal_layers(std::vector<layer_ptr>)), map, SLOT(on_layers(std::vector<layer_ptr>)));
  connect(tree, SIGNAL(signal_proj(brig::proj::shared_pj)), map, SLOT(on_proj(brig::proj::shared_pj)));
  connect(tree, SIGNAL(signal_rect(QRectF, brig::proj::shared_pj)), map, SLOT(on_rect(QRectF, brig::proj::shared_pj)));
  connect(tree, SIGNAL(signal_scale(double, brig::proj::shared_pj)), map, SLOT(on_scale(double, brig::proj::shared_pj)));
  connect(tree, SIGNAL(signal_task(std::shared_ptr<task>)), map, SLOT(on_task(std::shared_ptr<task>)));
  connect(tree, SIGNAL(signal_sql(provider_ptr, std::vector<std::string>)), sql, SLOT(on_sql(provider_ptr, std::vector<std::string>)));
  connect(tree, SIGNAL(signal_disconnect(provider_ptr)), sql, SLOT(on_disconnect(provider_ptr)));

  connect(map, SIGNAL(signal_task(std::shared_ptr<task>)), sql, SLOT(on_task(std::shared_ptr<task>)));
  connect(map, SIGNAL(signal_start()), this, SLOT(on_map_start()));
  connect(map, SIGNAL(signal_process(QString, bool)), this, SLOT(on_map_process(QString, bool)));
  connect(map, SIGNAL(signal_idle()), this, SLOT(on_map_idle()));
  connect(map, SIGNAL(signal_active(brig::proj::shared_pj)), this, SLOT(on_map_active(brig::proj::shared_pj)));
  connect(map, SIGNAL(signal_coords(QString)), this, SLOT(on_map_coords(QString)));

  connect(sql, SIGNAL(signal_start()), this, SLOT(on_sql_start()));
  connect(sql, SIGNAL(signal_process(QString, bool)), this, SLOT(on_sql_process(QString, bool)));
  connect(sql, SIGNAL(signal_idle()), this, SLOT(on_sql_idle()));
  connect(sql, SIGNAL(signal_active()), this, SLOT(on_sql_active()));

  setWindowIcon(QIcon(":/res/wheel.png"));
  setWindowTitle("brigantine");
  try  { on_map_active(latlon()); }
  catch (const std::exception&)  {}

  startTimer(100);
}

static QString status(bool limit, QString msg, const QTime* time = 0)
{
  static const int MessageLimit = 18;
  QString res;
  if (time) res.setNum((double)time->elapsed() / 1000., 'f', 1);
  if (limit && msg.size() > MessageLimit) msg = msg.left(MessageLimit) + "...";
  if (!res.isEmpty() && !msg.isEmpty()) res += " , ";
  res += msg;
  return res;
}

void main_window::on_map_active(brig::proj::shared_pj pj)
{
  const int epsg(get_epsg(pj));
  if (epsg < 0) m_proj_msg = QString( pj.get_def().c_str() );
  else m_proj_msg = QString("EPSG:%1").arg(epsg);

  if (!m_proj_msg.isEmpty()) m_tab->setCurrentIndex(m_map_tab);
  m_proj_stat->setDisabled(m_proj_msg.isEmpty());
  m_proj_stat->setText(rich_text(m_proj_msg.isEmpty()? ":/res/map_disabled.png": ":/res/map.png", status(true, m_proj_msg), false));
  m_proj_stat->setToolTip(m_proj_msg);
}

void main_window::on_map_coords(QString msg)
{
  m_pos_msg = msg;
  m_pos_stat->setDisabled(m_pos_msg.isEmpty());
  m_pos_stat->setText(rich_text(m_pos_msg.isEmpty()? ":/res/globe_disabled.png": ":/res/globe.png", m_pos_msg, false));
}

void main_window::on_map_start()
{
  m_map_msg = "";
  m_map_time.restart();
  m_map_stat->setToolTip("");
  m_map_stat->setEnabled(true);
}

void main_window::on_map_process(QString msg, bool done)
{
  m_map_msg = msg;
  if (done)
  {
    m_map_hist.push_front( status(false, m_map_msg, &m_map_time) );
    if (m_map_hist.size() > 5) m_map_hist.pop_back();
  }
}

void main_window::on_map_idle()
{
  m_map_stat->setDisabled(true);
  m_map_stat->setText(rich_text(":/res/map_disabled.png", status(true, m_map_msg, &m_map_time), true));
  m_map_stat->setToolTip(m_map_msg);
}

void main_window::on_sql_start()
{
  m_sql_msg = "";
  m_sql_time.restart();
  m_sql_stat->setToolTip("");
  m_sql_stat->setEnabled(true);
}

void main_window::on_sql_process(QString msg, bool done)
{
  m_sql_msg = msg;
  if (done)
  {
    m_sql_hist.push_front( status(false, m_sql_msg, &m_sql_time) );
    if (m_sql_hist.size() > 5) m_sql_hist.pop_back();
  }
}

void main_window::on_sql_idle()
{
  m_sql_stat->setDisabled(true);
  m_sql_stat->setText(rich_text(":/res/sql_disabled.png", status(true, m_sql_msg, &m_sql_time), true));
  m_sql_stat->setToolTip(m_sql_msg);
}

void main_window::timerEvent(QTimerEvent*)
{
  if (m_map_stat->isEnabled()) m_map_stat->setText(rich_text(":/res/map.png", status(true, m_map_msg, &m_map_time), true));
  if (m_sql_stat->isEnabled()) m_sql_stat->setText(rich_text(":/res/sql.png", status(true, m_sql_msg, &m_sql_time), true));
}

void main_window::on_show_stat_menu(QPoint point)
{
  QStatusBar* status_bar(statusBar());
  QWidget* widget(status_bar->childAt(point));
  QList<QAction*> actions;
  if (widget == m_proj_stat)
    actions.append(m_copy_proj_stat);
  else if (widget == m_map_stat)
    actions.append(m_copy_map_stat);
  else if (widget == m_sql_stat)
    actions.append(m_copy_sql_stat);
  if (!actions.empty())
    QMenu::exec(actions, status_bar->mapToGlobal(point));
}

void main_window::on_copy_proj_stat()
{
  QApplication::clipboard()->setText(m_proj_msg);
}

void main_window::on_copy_map_stat()
{
  QApplication::clipboard()->setText(m_map_hist.join("\n"));
}

void main_window::on_copy_sql_stat()
{
  QApplication::clipboard()->setText(m_sql_hist.join("\n"));
}

void main_window::keyPressEvent(QKeyEvent* event)
{
  using namespace std;
  if (event->key() == Qt::Key_F1)
  {
    QStringList props;
    props.append(QString("brigantine: %1, %2-bit").arg(__DATE__).arg(sizeof(void*) * 8));
    props.append(QString());
    props.append(QString("Qt: ") + qVersion());
    props.append(QString("Boost: %1.%2.%3").arg(BOOST_VERSION / 100000).arg((BOOST_VERSION / 100) % 1000).arg(BOOST_VERSION % 100));
    if (!brig::proj::version().empty()) props.append(QString::fromStdString("Proj: " + brig::proj::version()));
    if (!brig::gdal::version().empty()) props.append(QString::fromStdString("GDAL: " + brig::gdal::version()));
    if (!brig::osm::curl_version().empty()) props.append(QString::fromStdString("cURL: " + brig::osm::curl_version()));
    if (!brig::database::sqlite::sqlite3_libversion().empty()) props.append(QString::fromStdString("SQLite: " + brig::database::sqlite::sqlite3_libversion()));
    if (!brig::database::sqlite::spatialite_version().empty()) props.append(QString::fromStdString("SpatiaLite: " + brig::database::sqlite::spatialite_version()));
    if (!brig::database::mysql::client_version().empty()) props.append(QString::fromStdString("MySQL client: " + brig::database::mysql::client_version()));
    if (!brig::database::oracle::client_version().empty()) props.append(QString::fromStdString("Oracle client: " + brig::database::oracle::client_version()));
    if (!brig::database::postgres::client_version().empty()) props.append(QString::fromStdString("Postgres client: " + brig::database::postgres::client_version()));
    vector<string> drvs;
    brig::database::odbc::drivers(drvs);
    if (!drvs.empty())
    {
      props.append("ODBC: ");
      sort(begin(drvs), end(drvs));
      for (auto i(begin(drvs)); i != end(drvs); ++i)
        props.append(QString::fromStdString("- " + *i));
    }
    props.append(QString());
    props.append("andrew.naplavkov@gmail.com");
    props.append("lordnn@yahoo.com");
    QMessageBox::about(this, "about", props.join("\n"));
  }
  else
    QWidget::keyPressEvent(event);
}
