// Andrew Naplavkov

#ifndef DIALOG_CREATE_H
#define DIALOG_CREATE_H

#include <QDialog>
#include <string>

namespace Ui { class dialog_create; }

class dialog_create : public QDialog {
  Q_OBJECT
  Ui::dialog_create* ui;
  bool m_sql;
private slots:
  void on_create_button_clicked();
  void on_cancel_button_clicked();
  void on_sql_button_clicked();
public:
  dialog_create(QWidget* parent, const std::string& name);
  virtual ~dialog_create();
  std::string name() const;
  bool sql() const;
}; // dialog_create

#endif // DIALOG_CREATE_H
