// Andrew Naplavkov

#ifndef DIALOG_CLONE_H
#define DIALOG_CLONE_H

#include <QDialog>
#include <string>

namespace Ui { class dialog_clone; }

class dialog_clone : public QDialog {
  Q_OBJECT
  Ui::dialog_clone* ui;
  bool m_sql;
private slots:
  void on_clone_button_clicked();
  void on_cancel_button_clicked();
  void on_sql_button_clicked();
public:
  dialog_clone(const std::string& tbl);
  virtual ~dialog_clone();
  std::string tbl() const;
  bool sql() const;
}; // dialog_clone

#endif // DIALOG_CLONE_H
