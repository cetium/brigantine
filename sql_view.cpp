// Andrew Naplavkov

#include <brig/string_cast.hpp>
#include <exception>
#include <iterator>
#include <QAbstractButton>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <string>
#include <vector>
#include "global.h"
#include "provider.h"
#include "sql_view.h"
#include "task_fetch.h"
#include "task_tables.h"
#include "utilities.h"

sql_view::sql_view(QWidget* parent)
  : QWidget(parent), m_mdl(new rowset_model(0)), m_tasks(0)
{
  m_title = new QLabel;
  m_title->setTextFormat(Qt::RichText);

  QToolBar* tool_bar(new QToolBar);
  tool_bar->setOrientation(Qt::Horizontal);
  tool_bar->setFloatable(false);
  tool_bar->setMovable(false);

  m_open = tool_bar->addAction(QIcon(":/res/open.png"), "open", this, SLOT(on_open()));
  m_save = tool_bar->addAction(QIcon(":/res/save.png"), "save", this, SLOT(on_save()));
  m_run = tool_bar->addAction(QIcon(":/res/run.png"), "run", this, SLOT(on_run())); m_run->setDisabled(true);
  m_info = tool_bar->addAction(QIcon(":/res/info.png"), "info", this, SLOT(on_info())); m_info->setDisabled(true);

  m_sql = new QTextEdit;
  m_sql->setAcceptRichText(false);
  m_sql->setTabChangesFocus(true);

  m_rowset = new QTableView;
  m_rowset->setModel(m_mdl.get());
  m_rowset->verticalHeader()->hide();

  QSplitter* splitter(new QSplitter);
  splitter->setOrientation(Qt::Vertical);
  splitter->addWidget(m_sql);
  splitter->addWidget(m_rowset);

  QVBoxLayout* layout(new QVBoxLayout);
  layout->addWidget(m_title, 0, Qt::AlignCenter);
  layout->addWidget(tool_bar);
  layout->addWidget(splitter);
  setLayout(layout);
}

void sql_view::on_finished()
{
  --m_tasks;
  if (m_tasks == 0) emit signal_idle();
}

void sql_view::on_info()
{
  qRegisterMetaType<std::shared_ptr<rowset_model>>("std::shared_ptr<rowset_model>");
  task_tables* tsk(new task_tables(m_pvd));
  connect(tsk, SIGNAL(signal_finished()), this, SLOT(on_finished()));
  connect(tsk, SIGNAL(signal_rowset(std::shared_ptr<rowset_model>)), this, SLOT(on_rowset(std::shared_ptr<rowset_model>)));
  emit signal_task(std::shared_ptr<task>(tsk));
  ++m_tasks;
  emit signal_progress();
}

void sql_view::on_run()
{
  QMessageBox dlg(QApplication::activeWindow());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  dlg.setIcon(QMessageBox::Question);
  auto exec_btn(dlg.addButton("exec", QMessageBox::AcceptRole));
  auto fetch_btn(dlg.addButton("fetch", QMessageBox::AcceptRole));
  dlg.addButton("cancel", QMessageBox::RejectRole);
  dlg.setText("specify the SQL action to perform");
  dlg.exec();
  if (dlg.clickedButton() == static_cast<QAbstractButton*>(exec_btn))
  {
    class task_exec_batch : public task {
      provider_ptr m_pvd;
      std::string m_sql;
    public:
      task_exec_batch(provider_ptr pvd, const std::string& sql) : m_pvd(pvd), m_sql(sql)  {}
      QString get_string() override  { return limited_text(m_sql.c_str(), false); }
      int get_priority() override  { return 1; }
      void run_impl() override  { m_pvd->get_command()->exec_batch(m_sql); }
    }; // task_exec_batch

    task_exec_batch* tsk(new task_exec_batch(m_pvd, m_sql->toPlainText().toUtf8().constData()));
    connect(tsk, SIGNAL(signal_finished()), this, SLOT(on_finished()));
    emit signal_task(std::shared_ptr<task>(tsk));
    ++m_tasks;
    emit signal_progress();
  }
  else if (dlg.clickedButton() == static_cast<QAbstractButton*>(fetch_btn))
  {
    qRegisterMetaType<std::shared_ptr<rowset_model>>("std::shared_ptr<rowset_model>");
    task_fetch* tsk(new task_fetch(m_pvd, m_sql->toPlainText().toUtf8().constData()));
    connect(tsk, SIGNAL(signal_finished()), this, SLOT(on_finished()));
    connect(tsk, SIGNAL(signal_rowset(std::shared_ptr<rowset_model>)), this, SLOT(on_rowset(std::shared_ptr<rowset_model>)));
    emit signal_task(std::shared_ptr<task>(tsk));
    ++m_tasks;
    emit signal_progress();
  }
}

void sql_view::reset()
{
  m_pvd = provider_ptr();
  m_title->setText("");
  m_title->setToolTip("");
  m_run->setDisabled(true);
  m_info->setDisabled(true);
}

void sql_view::on_sql(provider_ptr pvd, std::vector<std::string> sqls)
{
  if (pvd->is_database())
  {
    m_pvd = pvd;
    m_title->setText(rich_text(m_pvd->get_icon(), limited_text(m_pvd->get_string(), true), false));
    m_title->setToolTip(m_pvd->get_string());
    m_run->setEnabled(true);
    m_info->setEnabled(true);
    for (auto sql(std::begin(sqls)); sql != std::end(sqls); ++sql)
    {
      m_sql->append("");
      m_sql->append(QString("-- %1").arg(QString::fromUtf8(sql->c_str())));
    }
    emit signal_sql();
  }
  else
    reset();
}

void sql_view::on_disconnect(provider_ptr pvd)
{
  if (pvd != m_pvd) return;
  reset();
}

void sql_view::on_rowset(std::shared_ptr<rowset_model> mdl)
{
  m_rowset->setModel(mdl.get());
  m_mdl = mdl;
  emit signal_sql();
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

  settings.setValue(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), QFileInfo(dlg.selectedFiles()[0]).absolutePath());
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

  settings.setValue(QString("%1/%2").arg(SettingsSQL).arg(SettingsPath), QFileInfo(dlg.selectedFiles()[0]).absolutePath());
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
