// Andrew Naplavkov

#include <brig/global.hpp>
#include <QString>
#include <QThreadPool>
#include "task_scheduler.h"

task_scheduler::task_scheduler(QObject* parent) : QObject(parent)
{
  QThreadPool::globalInstance()->setMaxThreadCount(brig::PoolSize);
}

void task_scheduler::on_task(std::shared_ptr<task> tsk)
{
  class task_ptr : public QRunnable {
    std::shared_ptr<task> m_tsk;
  public:
    explicit task_ptr(std::shared_ptr<task> tsk) : m_tsk(tsk)  {}
    void run() override  { m_tsk->run(); }
  }; // task_ptr

  QThreadPool::globalInstance()->start(new task_ptr(tsk), tsk->get_priority());
}
