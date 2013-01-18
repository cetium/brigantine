// Andrew Naplavkov

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include "frame.h"

struct progress;

class task : public QObject {
  Q_OBJECT
protected:
  frame m_fr;
public:
  void set_frame(const frame& fr)  { m_fr = fr; }
  virtual void run(progress*) = 0;
}; // task

#endif // TASK_H
