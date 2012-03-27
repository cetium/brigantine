// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <QHeaderView>
#include <QSplitter>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <vector>
#include <string>
#include "connection.h"
#include "progress.h"
#include "sql_view.h"
#include "task_exec.h"
#include "utilities.h"

sql_view::sql_view(QWidget* parent) : QWidget(parent), m_mdl(new sql_model()), m_trd(m_mdl)
{
  m_title = new QLabel;
  m_title->setTextFormat(Qt::RichText);

  QToolBar* tool_bar(new QToolBar);
  tool_bar->setOrientation(Qt::Horizontal);
  tool_bar->setFloatable(false);
  tool_bar->setMovable(false);

  m_fetch = tool_bar->addAction(QIcon(":/res/fetch.png"), "fetch", this, SLOT(fetch()));
  m_fetch->setDisabled(true);

  m_run = tool_bar->addAction(QIcon(":/res/run.png"), "run", this, SLOT(run()));
  m_run->setDisabled(true);

  m_cancel = tool_bar->addAction(QIcon(":/res/delete.png"), "stop", this, SLOT(cancel()));
  m_cancel->setDisabled(true);

  m_sql = new QTextEdit;
  m_sql->setAcceptRichText(false);
  m_sql->setTabChangesFocus(true);

  QTableView* table(new QTableView);
  table->setModel(m_mdl.get());
  table->verticalHeader()->hide();

  QSplitter* splitter(new QSplitter);
  splitter->setOrientation(Qt::Vertical);
  splitter->addWidget(m_sql);
  splitter->addWidget(table);

  QVBoxLayout* layout(new QVBoxLayout);
  layout->addWidget(m_title, 0, Qt::AlignCenter);
  layout->addWidget(tool_bar);
  layout->addWidget(splitter);
  setLayout(layout);

  connect(&m_trd, SIGNAL(signal_start()), this, SLOT(on_start()));
  connect(&m_trd, SIGNAL(signal_process(QString)), this, SLOT(on_process(QString)));
  connect(&m_trd, SIGNAL(signal_idle()), this, SLOT(on_idle()));
}

void sql_view::fetch()
{
  struct select : task {
    connection_link dbc;
    std::string sql;

    virtual void run(progress* prg)
    {
      auto cmd(dbc->get_command());
      cmd->exec(sql);
      prg->init(cmd->columns());
      std::vector<brig::database::variant> row;
      for (size_t counter(1); cmd->fetch(row); ++counter)
      {
        std::vector<std::string> str_row;
        for (auto iter(std::begin(row)); iter != std::end(row); ++iter)
          str_row.push_back(brig::string_cast<char>(*iter));
        if (!prg->step(counter, str_row)) return;
      }
    }
  }; // select

  select* tsk(new select());
  tsk->dbc = m_dbc;
  tsk->sql = m_sql->toPlainText().toUtf8().constData();
  m_trd.push(std::shared_ptr<task>(tsk));
}

void sql_view::run()
{
  task_exec* tsk(new task_exec(m_dbc, std::vector<std::string>(1, m_sql->toPlainText().toUtf8().constData())));
  m_trd.push(std::shared_ptr<task>(tsk));
}

void sql_view::cancel()  { m_trd.cancel(); }

void sql_view::push(std::shared_ptr<task> tsk)  { m_trd.push(tsk); }

void sql_view::on_commands(connection_link dbc, std::vector<std::string> sqls)
{
  m_dbc = dbc;

  static const int TitleLimit = 40;
  QString title = m_dbc->get_string();
  if (title.size() > TitleLimit) title = "..." + title.right(TitleLimit);
  m_title->setText(rich_text(m_dbc->get_icon(), title));
  m_title->setToolTip(m_dbc->get_string());

  m_fetch->setDisabled(m_cancel->isEnabled());
  m_run->setDisabled(m_cancel->isEnabled());
  if (!sqls.empty()) m_sql->append("");
  for (auto sql(std::begin(sqls)); sql != std::end(sqls); ++sql)
    m_sql->append("-- " + QString::fromUtf8(sql->c_str()));
  emit signal_commands();
}

void sql_view::on_disconnect(connection_link dbc)
{
  if (dbc != m_dbc) return;
  m_dbc = connection_link();
  m_title->setText("");
  m_title->setToolTip("");
  m_fetch->setDisabled(true);
  m_run->setDisabled(true);
}

void sql_view::on_start()
{
  m_fetch->setDisabled(true);
  m_run->setDisabled(true);
  m_cancel->setEnabled(true);
  emit signal_start();
}

void sql_view::on_process(const QString& msg)  { emit signal_process(msg); }

void sql_view::on_idle()
{
  m_fetch->setEnabled(m_dbc);
  m_run->setEnabled(m_dbc);
  m_cancel->setDisabled(true);
  emit signal_idle();
}
