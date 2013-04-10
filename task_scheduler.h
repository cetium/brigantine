// Andrew Naplavkov

#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <memory>
#include <QObject>
#include "task.h"

class task_scheduler : public QObject {
  Q_OBJECT
public slots:
  void on_task(std::shared_ptr<task> tsk);
public:
  explicit task_scheduler(QObject* parent);
}; // task_scheduler

#endif // TASK_SCHEDULER_H
