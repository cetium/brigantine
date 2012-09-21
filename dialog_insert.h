// Andrew Naplavkov

#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QCheckBox>
#include <QDialog>
#include <QIntValidator>
#include <QTabWidget>
#include <string>
#include <vector>
#include "insert_item.h"
#include "layer_link.h"

class dialog_insert : public QDialog {
  Q_OBJECT
  QIntValidator m_vlr;
  QTabWidget* m_tab;
  QCheckBox* m_ccw;

public:
  dialog_insert(QWidget* parent, layer_link lr_from, layer_link lr_to);
  std::vector<insert_item> get_items() const;
  bool ccw() const  { return Qt::Checked == m_ccw->checkState(); }
}; // dialog_insert

#endif // DIALOG_INSERT_H
