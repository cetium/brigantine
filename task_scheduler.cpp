// Andrew Naplavkov

#include <brig/global.hpp>
#include <QString>
#include <QThread>
#include <QThreadPool>
#include "task_scheduler.h"

task_scheduler::task_scheduler(QObject* parent) : QObject(parent)
{
  int threads(QThread::idealThreadCount());
  if (threads <= 0 || threads > int(brig::PoolSize)) threads = int(brig::PoolSize);
  QThreadPool::globalInstance()->setMaxThreadCount(threads);
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
