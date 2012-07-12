// Andrew Naplavkov

#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QDialog>
#include <QRegExpValidator>
#include <string>
#include <vector>
#include "insert_item.h"
#include "layer_link.h"

class dialog_insert : public QDialog {
  Q_OBJECT
  QRegExpValidator m_vlr;
public:
  dialog_insert(QWidget* parent, layer_link lr_from, layer_link lr_to);
  std::vector<insert_item> get_items() const;
}; // dialog_insert

#endif // DIALOG_INSERT_H
