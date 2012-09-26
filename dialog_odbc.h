// Andrew Naplavkov

#ifndef DIALOG_ODBC_H
#define DIALOG_ODBC_H

#include <QDialog>
#include <QString>

namespace Ui { class dialog_odbc; }

class dialog_odbc : public QDialog {
  Q_OBJECT
  Ui::dialog_odbc* ui;
private slots:
  void on_cancel_button_clicked()  { reject(); }
  void on_connect_button_clicked();
  void on_file_button_clicked();
public:
  explicit dialog_odbc(QWidget* parent);
  virtual ~dialog_odbc();
  QString str() const;
}; // dialog_odbc

#endif // DIALOG_ODBC_H
