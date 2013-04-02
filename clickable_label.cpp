// Andrew Naplavkov

#include "clickable_label.h"

void clickable_label::mousePressEvent(QMouseEvent* evt)
{
  emit clicked();
  QLabel::mousePressEvent(evt);
}
