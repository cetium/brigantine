// Andrew Naplavkov

#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>
#include <QMouseEvent>

class clickable_label : public QLabel {
  Q_OBJECT
protected:
  void mousePressEvent(QMouseEvent* evt) override;
signals:
  void clicked();
}; // clickable_label

#endif // CLICKABLE_LABEL_H
