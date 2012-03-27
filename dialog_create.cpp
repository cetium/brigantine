// Andrew Naplavkov

#include <QApplication>
#include <QString>
#include "dialog_create.h"
#include "ui_dialog_create.h"

dialog_create::dialog_create(const std::string& tbl)
  : QDialog(QApplication::activeWindow()), ui(new Ui::dialog_create), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->table_edit->setText(QString::fromUtf8(tbl.c_str()));
}

dialog_create::~dialog_create()  { delete ui; }
std::string dialog_create::tbl() const  { return ui->table_edit->text().toUtf8().constData(); }
bool dialog_create::sql() const  { return m_sql; }
void dialog_create::on_create_button_clicked()  { accept(); }
void dialog_create::on_cancel_button_clicked()  { reject(); }
void dialog_create::on_sql_button_clicked()  { m_sql = true; accept(); }
