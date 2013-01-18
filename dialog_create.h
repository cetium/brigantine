// Andrew Naplavkov

#ifndef DIALOG_CREATE_H
#define DIALOG_CREATE_H

#include <QDialog>
#include <QString>

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
  dialog_create(QWidget* parent, const QString& name, bool sql);
  ~dialog_create() override;
  bool sql() const;
  bool view() const;
}; // dialog_create

#endif // DIALOG_CREATE_H
