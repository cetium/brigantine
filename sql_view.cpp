// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <exception>
#include <iterator>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QSettings>
#include <QSplitter>
#include <QTableWidget>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <string>
#include <vector>
#include "connection.h"
#include "global.h"
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

  m_open = tool_bar->addAction(QIcon(":/res/open.png"), "open", this, SLOT(on_open()));
  m_save = tool_bar->addAction(QIcon(":/res/save.png"), "save", this, SLOT(on_save()));
  m_fetch = tool_bar->addAction(QIcon(":/res/fetch.png"), "fetch", this, SLOT(on_fetch())); m_fetch->setDisabled(true);
  m_run = tool_bar->addAction(QIcon(":/res/run.png"), "run", this, SLOT(on_run())); m_run->setDisabled(true);
  m_cancel = tool_bar->addAction(QIcon(":/res/delete.png"), "stop", this, SLOT(on_cancel())); m_cancel->setDisabled(true);
  m_info = tool_bar->addAction(QIcon(":/res/info.png"), "info", this, SLOT(on_info())); m_info->setDisabled(true);

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
  connect(&m_trd, SIGNAL(signal_process(QString)), this, SLOT(emit_process(QString)));
  connect(&m_trd, SIGNAL(signal_idle()), this, SLOT(on_idle()));
}

void sql_view::on_info()
{
  struct tables : task {
    connection_link dbc;

    void run(progress* prg) override
    {
      std::vector<std::string> row;
      row.push_back("SCHEMA");
      row.push_back("TABLE");
      prg->init(row);

      auto ids(dbc->get_tables());
      size_t counter(1);
      for (auto id(std::begin(ids)); id != std::end(ids); ++id, ++counter)
      {
        row[0] = id->schema;
        row[1] = id->name;
        if (!prg->step(counter, row)) return;
      }
    }
  }; // tables

  tables* tsk(new tables());
  tsk->dbc = m_dbc;
  m_trd.push(std::shared_ptr<task>(tsk));
}

void sql_view::on_fetch()
{
  struct select : task {
    connection_link dbc;
    std::string sql;

    void run(progress* prg) override
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

void sql_view::on_run()
{
  task_exec* tsk(new task_exec(m_dbc, m_sql->toPlainText().toUtf8().constData()));
  m_trd.push(std::shared_ptr<task>(tsk));
}

void sql_view::on_commands(connection_link dbc, std::vector<std::string> sqls)
{
  m_dbc = dbc;

  static const int TitleLimit = 40;
  QString title = m_dbc->get_string();
  if (title.size() > TitleLimit) title = "..." + title.right(TitleLimit);
  m_title->setText(rich_text(m_dbc->get_icon(), title, false));
  m_title->setToolTip(m_dbc->get_string());

  m_fetch->setDisabled(m_cancel->isEnabled());
  m_run->setDisabled(m_cancel->isEnabled());
  m_info->setDisabled(m_cancel->isEnabled());
  for (auto sql(std::begin(sqls)); sql != std::end(sqls); ++sql)
  {
    m_sql->append("");
    m_sql->append("-- " + QString::fromUtf8(sql->c_str()));
  }
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
  m_info->setDisabled(true);
}

void sql_view::on_start()
{
  m_fetch->setDisabled(true);
  m_run->setDisabled(true);
  m_cancel->setEnabled(true);
  m_info->setDisabled(true);
  emit signal_start();
}

void sql_view::on_idle()
{
  m_fetch->setEnabled(m_dbc);
  m_run->setEnabled(m_dbc);
  m_cancel->setDisabled(true);
  m_info->setEnabled(m_dbc);
  emit signal_idle();
}

void sql_view::on_open()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  QFileDialog dlg
  ( this
  , "open SQL file"
  , settings.value(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), QDir::currentPath()).toString()
  , "SQL files (*.sql);;All files (*.*)"
  );
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFile);
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (dlg.exec() != QDialog::Accepted) return;
  settings.setValue(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), dlg.directory().absolutePath());
  QFile file(dlg.selectedFiles()[0]);
  if (!file.open(QFile::ReadOnly)) return;
  QTextStream stream(&file);
  m_sql->setText(stream.readAll());
}

void sql_view::on_save()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  QFileDialog dlg
    ( this
    , "new SQL file"
    , settings.value(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), QDir::currentPath()).toString()
    , "SQL files (*.sql)"
    );
  dlg.setAcceptMode(QFileDialog::AcceptSave);
  dlg.setFileMode(QFileDialog::AnyFile);
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (!dlg.exec()) return;
  settings.setValue(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), dlg.directory().absolutePath());
  QFileInfo info(dlg.selectedFiles().value(0));
  if (info.suffix().isEmpty()) info = QFileInfo(info.dir(), info.fileName() + ".sql");
  if (info.exists() && !QFile::remove(info.filePath()))
  {
    show_message("file removing error");
    return;
  }
  QFile file(info.filePath());
  if (!file.open(QFile::WriteOnly)) return;
  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << m_sql->toPlainText().toUtf8().constData();
}
