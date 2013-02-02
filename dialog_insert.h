// Andrew Naplavkov

#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QCheckBox>
#include <QDialog>
#include <QTabWidget>
#include <string>
#include <vector>
#include "insert_item.h"
#include "layer_ptr.h"

class dialog_insert : public QDialog {
  Q_OBJECT
  QTabWidget* m_tab;
  QCheckBox *m_ccw, *m_view;
public:
  dialog_insert(QWidget* parent, layer_ptr lr_from, layer_ptr lr_to);
  std::vector<insert_item> get_items() const;
  bool ccw() const  { return m_ccw->isChecked(); }
  bool view() const  { return m_view->isChecked(); }
}; // dialog_insert

#endif // DIALOG_INSERT_H
