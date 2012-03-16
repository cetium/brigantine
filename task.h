// Andrew Naplavkov

#ifndef TASK_H
#define TASK_H

#include <QObject>

struct progress;

class task : public QObject {
  Q_OBJECT
public:
  virtual void run(progress*) = 0;
}; // task

#endif // TASK_H
