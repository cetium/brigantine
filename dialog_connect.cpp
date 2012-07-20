// Andrew Naplavkov

#include "dialog_connect.h"
#include "ui_dialog_connect.h"

dialog_connect::dialog_connect(QWidget* parent, QString icon, QString host, int port, QString db, QString usr, QString pwd)
  : QDialog(parent), ui(new Ui::dialog_connect)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setWindowIcon(QIcon(icon));
  ui->host_edit->setText(host);
  ui->port_edit->setText(QString().setNum(port));
  ui->db_edit->setText(db);
  ui->usr_edit->setText(usr);
  ui->pwd_edit->setText(pwd);
}

dialog_connect::~dialog_connect()  { delete ui; }
QString dialog_connect::host() const  { return ui->host_edit->text(); }
int dialog_connect::port() const  { return ui->port_edit->text().toInt(); }
QString dialog_connect::db() const  { return ui->db_edit->text(); }
QString dialog_connect::usr() const  { return ui->usr_edit->text(); }
QString dialog_connect::pwd() const  { return ui->pwd_edit->text(); }
