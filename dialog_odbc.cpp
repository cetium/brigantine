// Andrew Naplavkov

#include <brig/database/odbc/datasources.hpp>
#include <QApplication>
#include <QFileDialog>
#include "dialog_odbc.h"
#include "ui_odbc.h"

dialog_odbc::dialog_odbc() : QDialog(QApplication::activeWindow()), ui(new Ui::odbc)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  std::vector<std::string> dsns;
  brig::database::odbc::data_sources(dsns);
  for (size_t i(0); i < dsns.size(); ++i)
    ui->comboBox->addItem("DSN=" + QString::fromUtf8(dsns[i].c_str()) + ";");
}

void dialog_odbc::on_file_button_clicked()
{
  QFileDialog dlg(this, "open DSN file", QDir::currentPath(), "DSN files (*.dsn)");
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFile);
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  if (dlg.exec() != QDialog::Accepted) return;
  ui->comboBox->setEditText("FILEDSN=" + dlg.selectedFiles().value(0) + ";");
}

dialog_odbc::~dialog_odbc()  { delete ui; }
QString dialog_odbc::str() const  { return ui->comboBox->currentText(); }
