// Andrew Naplavkov

#include <QString>
#include "dialog_create.h"
#include "ui_dialog_create.h"

dialog_create::dialog_create(QWidget* parent, const std::string& name)
  : QDialog(parent), ui(new Ui::dialog_create), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (name.empty())
  {
    ui->name_label->setVisible(false);
    ui->name_edit->setVisible(false);
  }
  else
    ui->name_edit->setText(QString::fromUtf8(name.c_str()));
}

dialog_create::~dialog_create()  { delete ui; }
std::string dialog_create::name() const  { return ui->name_edit->text().toUtf8().constData(); }
bool dialog_create::sql() const  { return m_sql; }
void dialog_create::on_create_button_clicked()  { accept(); }
void dialog_create::on_cancel_button_clicked()  { reject(); }
void dialog_create::on_sql_button_clicked()  { m_sql = true; accept(); }
