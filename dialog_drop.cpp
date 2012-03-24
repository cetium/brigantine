// Andrew Naplavkov

#include <QApplication>
#include <QString>
#include "dialog_drop.h"
#include "ui_dialog_drop.h"

dialog_drop::dialog_drop(const QString& lr)
  : QDialog(QApplication::activeWindow()), ui(new Ui::dialog_drop), m_sql(false)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->label->setText("do you want to drop <b>" + lr + "</b> ?");
}

dialog_drop::~dialog_drop()  { delete ui; }
bool dialog_drop::sql() const  { return m_sql; }
void dialog_drop::on_drop_button_clicked()  { accept(); }
void dialog_drop::on_cancel_button_clicked()  { reject(); }
void dialog_drop::on_sql_button_clicked()  { m_sql = true; accept(); }
