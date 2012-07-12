// Andrew Naplavkov

#ifndef dialog_connect_H
#define dialog_connect_H

#include <QDialog>
#include <QString>

namespace Ui { class dialog_connect; }

class dialog_connect : public QDialog {
  Q_OBJECT
  Ui::dialog_connect* ui;
private slots:
  void on_cancel_button_clicked()  { reject(); }
  void on_connect_button_clicked()  { accept(); }
public:
  dialog_connect(QWidget* parent, QString icon, QString host, int port, QString db, QString usr);
  virtual ~dialog_connect();
  QString host() const;
  int port() const;
  QString db() const;
  QString usr() const;
  QString pwd() const;
}; // dialog_connect

#endif // dialog_connect_H
