// Andrew Naplavkov

#ifndef DIALOG_DO_H
#define DIALOG_DO_H

#include <QDialog>
#include <QString>

namespace Ui { class dialog_do; }

class dialog_do : public QDialog {
  Q_OBJECT
  Ui::dialog_do* ui;
  bool m_sql;
private slots:
  void on_do_button_clicked();
  void on_cancel_button_clicked();
  void on_sql_button_clicked();
public:
  dialog_do(QWidget* parent, const QString& verb, const QString& name);
  ~dialog_do() override;
  bool sql() const;
}; // dialog_do

#endif // DIALOG_DO_H
