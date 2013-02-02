// Andrew Naplavkov

#include <brig/database/odbc/datasources.hpp>
#include <QDir>
#include <QFileDialog>
#include <QRegExp>
#include <QSettings>
#include "dialog_odbc.h"
#include "global.h"
#include "ui_dialog_odbc.h"

dialog_odbc::dialog_odbc(QWidget* parent) : QDialog(parent), ui(new Ui::dialog_odbc)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  std::vector<std::string> dsns;
  brig::database::odbc::datasources(dsns);
  for (size_t i(0); i < dsns.size(); ++i)
    ui->comboBox->addItem(QString("DSN=%1;").arg(QString::fromUtf8(dsns[i].c_str())));

  QSettings settings(SettingsIni, QSettings::IniFormat);
  ui->comboBox->setEditText(settings.value(QString("%1/%2").arg(SettingsODBC).arg(SettingsStr), "").toString());
}

dialog_odbc::~dialog_odbc()
{
  delete ui;
}

void dialog_odbc::on_file_button_clicked()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  QFileDialog dlg
    ( this
    , "open DSN file"
    , settings.value(QString("%1/%2").arg(SettingsODBC).arg(SettingsPath), QDir::currentPath()).toString()
    , "DSN files (*.dsn)"
    );
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFile);
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (dlg.exec() != QDialog::Accepted) return;

  settings.setValue(QString("%1/%2").arg(SettingsODBC).arg(SettingsPath), QFileInfo(dlg.selectedFiles()[0]).absolutePath());
  ui->comboBox->setEditText("FILEDSN=" + dlg.selectedFiles()[0] + ";");
}

void dialog_odbc::on_connect_button_clicked()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  QString s = str();
  s.replace(QRegExp("PWD=\\w*;"), "PWD=;");
  settings.setValue(QString("%1/%2").arg(SettingsODBC).arg(SettingsStr), s);
  accept();
}

QString dialog_odbc::str() const
{
  return ui->comboBox->currentText();
}
