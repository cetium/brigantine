// Andrew Naplavkov

#include <QApplication>
#include "dialog_oci.h"
#include "ui_oci.h"

dialog_oci::dialog_oci() : QDialog(QApplication::activeWindow()), ui(new Ui::oci)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

dialog_oci::~dialog_oci()  { delete ui; }
QString dialog_oci::srv() const  { return ui->server_edit->text(); }
QString dialog_oci::usr() const  { return ui->user_edit->text(); }
QString dialog_oci::pwd() const  { return ui->password_edit->text(); }
