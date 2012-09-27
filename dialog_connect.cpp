// Andrew Naplavkov

#include <QIntValidator>
#include <QSettings>
#include "dialog_connect.h"
#include "global.h"
#include "ui_dialog_connect.h"

dialog_connect::dialog_connect(QWidget* parent, QIcon icon, QString settings_prefix, QString host, int port, QString db, QString usr)
  : QDialog(parent), ui(new Ui::dialog_connect), m_settings_prefix(settings_prefix)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setWindowIcon(icon);
  QSettings settings(SettingsIni, QSettings::IniFormat);
  ui->host_edit->setText(settings.value(QString("%1/%2").arg(m_settings_prefix).arg(SettingsHost), host).toString());
  ui->port_edit->setValidator(new QIntValidator(0, INT_MAX, this));
  ui->port_edit->setText(settings.value(QString("%1/%2").arg(m_settings_prefix).arg(SettingsPort), port).toString());
  ui->db_edit->setText(settings.value(QString("%1/%2").arg(m_settings_prefix).arg(SettingsDb), db).toString());
  ui->usr_edit->setText(settings.value(QString("%1/%2").arg(m_settings_prefix).arg(SettingsUsr), usr).toString());
}

void dialog_connect::on_connect_button_clicked()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  settings.setValue(QString("%1/%2").arg(m_settings_prefix).arg(SettingsHost), host());
  settings.setValue(QString("%1/%2").arg(m_settings_prefix).arg(SettingsPort), port());
  settings.setValue(QString("%1/%2").arg(m_settings_prefix).arg(SettingsDb), db());
  settings.setValue(QString("%1/%2").arg(m_settings_prefix).arg(SettingsUsr), usr());
  accept();
}

dialog_connect::~dialog_connect()  { delete ui; }
QString dialog_connect::host() const  { return ui->host_edit->text(); }
int dialog_connect::port() const  { return ui->port_edit->text().toInt(); }
QString dialog_connect::db() const  { return ui->db_edit->text(); }
QString dialog_connect::usr() const  { return ui->usr_edit->text(); }
QString dialog_connect::pwd() const  { return ui->pwd_edit->text(); }
