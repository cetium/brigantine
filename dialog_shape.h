// Andrew Naplavkov

#ifndef DIALOG_SHAPE_H
#define DIALOG_SHAPE_H

#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QString>

class dialog_shape : public QFileDialog {
  Q_OBJECT
  QRegExpValidator m_vlr;
  QComboBox* m_charset_combo;
  QLineEdit* m_epsg_edit;
public:
  dialog_shape();
  QString charset();
  QString epsg();
}; // dialog_shape

#endif // DIALOG_SHAPE_H
