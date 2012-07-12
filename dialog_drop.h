// Andrew Naplavkov

#ifndef DIALOG_DROP_H
#define DIALOG_DROP_H

#include <QDialog>
#include <QString>

namespace Ui { class dialog_drop; }

class dialog_drop : public QDialog {
  Q_OBJECT
  Ui::dialog_drop* ui;
  bool m_sql;
private slots:
  void on_drop_button_clicked();
  void on_cancel_button_clicked();
  void on_sql_button_clicked();
public:
  dialog_drop(QWidget* parent, const QString& lr);
  virtual ~dialog_drop();
  bool sql() const;
}; // dialog_drop

#endif // DIALOG_DROP_H
