// Andrew Naplavkov

#include <QString>
#include "dialog_do.h"
#include "ui_dialog_do.h"

dialog_do::dialog_do(QWidget* parent, const QString& verb, const QString& name)
  : QDialog(parent), ui(new Ui::dialog_do), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->label->setText(QString("do you want to %1 %2?").arg(verb).arg(name));
  ui->do_button->setText(verb);
}

dialog_do::~dialog_do()  { delete ui; }
bool dialog_do::sql() const  { return m_sql; }
void dialog_do::on_do_button_clicked()  { accept(); }
void dialog_do::on_cancel_button_clicked()  { reject(); }
void dialog_do::on_sql_button_clicked()  { m_sql = true; accept(); }
