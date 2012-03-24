// Andrew Naplavkov

#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QDialog>
#include <QRegExpValidator>
#include <string>
#include "insert_map.h"
#include "layer_link.h"

class dialog_insert : public QDialog {
  Q_OBJECT
  QRegExpValidator m_vlr;
public:
  dialog_insert(layer_link lr_from, layer_link lr_to);
  insert_map get_insert_map() const;
}; // dialog_insert

#endif // DIALOG_INSERT_H
