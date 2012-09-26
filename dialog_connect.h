// Andrew Naplavkov

#ifndef dialog_connect_H
#define dialog_connect_H

#include <QDialog>
#include <QIcon>
#include <QString>

namespace Ui { class dialog_connect; }

class dialog_connect : public QDialog {
  Q_OBJECT
  Ui::dialog_connect* ui;
  QString m_settings_prefix;
private slots:
  void on_cancel_button_clicked()  { reject(); }
  void on_connect_button_clicked();
public:
  dialog_connect(QWidget* parent, QIcon icon, QString settings_prefix, QString host, int port, QString db, QString usr);
  virtual ~dialog_connect();
  QString host() const;
  int port() const;
  QString db() const;
  QString usr() const;
  QString pwd() const;
}; // dialog_connect

#endif // dialog_connect_H
