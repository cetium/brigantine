// Andrew Naplavkov

#ifndef DIALOG_ODBC_H
#define DIALOG_ODBC_H

#include <QDialog>
#include <QString>

namespace Ui { class odbc; }

class dialog_odbc : public QDialog {
  Q_OBJECT
  Ui::odbc* ui;
private slots:
  void on_cancel_button_clicked()  { reject(); }
  void on_connect_button_clicked()  { accept(); }
  void on_file_button_clicked();
public:
  dialog_odbc();
  virtual ~dialog_odbc();
  QString str() const;
}; // dialog_odbc

#endif // DIALOG_ODBC_H
