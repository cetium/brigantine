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
#include <QTableWidget>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <string>
#include <vector>
#include "global.h"
#include "provider.h"
#include "sql_view.h"
#include "utilities.h"

sql_view::sql_view(QWidget* parent)
  : QWidget(parent), m_mdl(new sql_model()), m_tasks(0)
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
}

void sql_view::on_finished(QString)
{
  --m_tasks;
  if (m_tasks == 0) emit signal_idle();
}

void sql_view::on_info()
{
  // todo:
}

void sql_view::on_run()
{
  QMessageBox dlg(QApplication::activeWindow());
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  dlg.setWindowIcon(QIcon(":/res/wheel.png"));
  auto exec_btn(dlg.addButton("exec", QMessageBox::AcceptRole));
  auto fetch_btn(dlg.addButton("fetch", QMessageBox::AcceptRole));
  dlg.addButton("cancel", QMessageBox::RejectRole);
  dlg.setText("specify the SQL action to perform");
  dlg.exec();
  if (dlg.clickedButton() == static_cast<QAbstractButton*>(exec_btn))
  {
    struct exec_batch : task {
      provider_ptr pvd;
      std::string sql;
      QString get_string() override  { return limited_text(sql.c_str(), false); }
      int get_priority() override  { return 1; }
      void do_run() override  { pvd->get_command()->exec_batch(sql); }
    }; // exec_batch

    exec_batch* tsk(new exec_batch());
    tsk->pvd = m_pvd;
    tsk->sql = m_sql->toPlainText().toUtf8().constData();
    connect(tsk, SIGNAL(signal_finished(QString)), this, SLOT(on_finished(QString)));
    emit signal_task(std::shared_ptr<task>(tsk));
    ++m_tasks;
    emit signal_progress();
  }
  else if (dlg.clickedButton() == static_cast<QAbstractButton*>(fetch_btn))
  {
    // todo:
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
    m_info->setDisabled(true);
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
