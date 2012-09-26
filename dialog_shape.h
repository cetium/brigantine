// Andrew Naplavkov

#ifndef DIALOG_SHAPE_H
#define DIALOG_SHAPE_H

#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QString>

class dialog_shape : public QFileDialog {
  Q_OBJECT
  QComboBox* m_charset_combo;
  QLineEdit* m_epsg_edit;
public slots:
  virtual void accept();
public:
  explicit dialog_shape(QWidget* parent);
  QString charset();
  QString epsg();
}; // dialog_shape

#endif // DIALOG_SHAPE_H
