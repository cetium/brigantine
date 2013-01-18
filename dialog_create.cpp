// Andrew Naplavkov

#include <QString>
#include "dialog_create.h"
#include "ui_dialog_create.h"

dialog_create::dialog_create(QWidget* parent, const QString& name, bool sql)
  : QDialog(parent), ui(new Ui::dialog_create), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->label->setText(QString("do you want to create %1?").arg(name));
  ui->sql_button->setEnabled(sql);
}

dialog_create::~dialog_create()  { delete ui; }
bool dialog_create::sql() const  { return m_sql; }
bool dialog_create::view() const  { return ui->view_checkbox->isChecked(); }
void dialog_create::on_create_button_clicked()  { accept(); }
void dialog_create::on_cancel_button_clicked()  { reject(); }
void dialog_create::on_sql_button_clicked()  { m_sql = true; accept(); }
