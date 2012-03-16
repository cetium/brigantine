// Andrew Naplavkov

#ifndef DIALOG_OCI_H
#define DIALOG_OCI_H

#include <QDialog>
#include <QString>

namespace Ui { class oci; }

class dialog_oci : public QDialog {
  Q_OBJECT
  Ui::oci* ui;
private slots:
  void on_cancel_button_clicked()  { reject(); }
  void on_connect_button_clicked()  { accept(); }
public:
  dialog_oci();
  virtual ~dialog_oci();
  QString srv() const;
  QString usr() const;
  QString pwd() const;
}; // dialog_oci

#endif // DIALOG_OCI_H
