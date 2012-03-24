// Andrew Naplavkov

#include <QApplication>
#include <QString>
#include "dialog_clone.h"
#include "ui_dialog_clone.h"

dialog_clone::dialog_clone(const std::string& tbl)
  : QDialog(QApplication::activeWindow()), ui(new Ui::dialog_clone), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->table_edit->setText(QString::fromUtf8(tbl.c_str()));
}

dialog_clone::~dialog_clone()  { delete ui; }
std::string dialog_clone::tbl() const  { return ui->table_edit->text().toUtf8().constData(); }
bool dialog_clone::sql() const  { return m_sql; }
void dialog_clone::on_clone_button_clicked()  { accept(); }
void dialog_clone::on_cancel_button_clicked()  { reject(); }
void dialog_clone::on_sql_button_clicked()  { m_sql = true; accept(); }
