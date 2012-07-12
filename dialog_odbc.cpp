// Andrew Naplavkov

#include <brig/database/odbc/datasources.hpp>
#include "dialog_odbc.h"
#include "ui_dialog_odbc.h"

dialog_odbc::dialog_odbc(QWidget* parent) : QDialog(parent), ui(new Ui::dialog_odbc)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  std::vector<std::string> dsns;
  brig::database::odbc::datasources(dsns);
  for (size_t i(0); i < dsns.size(); ++i)
    ui->comboBox->addItem("DSN=" + QString::fromUtf8(dsns[i].c_str()) + ";");

  m_dlg = new QFileDialog(this, "open DSN file", QDir::currentPath(), "DSN files (*.dsn)");
  m_dlg->setAcceptMode(QFileDialog::AcceptOpen);
  m_dlg->setFileMode(QFileDialog::ExistingFile);
  m_dlg->setWindowFlags(m_dlg->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void dialog_odbc::on_file_button_clicked()
{
  if (m_dlg->exec() != QDialog::Accepted) return;
  ui->comboBox->setEditText("FILEDSN=" + m_dlg->selectedFiles().value(0) + ";");
}

dialog_odbc::~dialog_odbc()  { delete ui; }
QString dialog_odbc::str() const  { return ui->comboBox->currentText(); }
