// Andrew Naplavkov

#include <exception>
#include <QMutexLocker>
#include <QTime>
#include "progress.h"
#include "sql_thread.h"
#include "utilities.h"

sql_thread::sql_thread(std::shared_ptr<sql_model> mdl) : m_mdl(mdl)
{
  // todo: MSVC November 2012 CTP - atomic_bool constructor problem
  m_abort = false;
  m_cancel = false;
}

sql_thread::~sql_thread()
{
  m_mutex.lock();
  m_abort = true;
  m_condition.wakeOne();
  m_mutex.unlock();

  if (!wait(TimeOut))
  {
    terminate();
    wait();
  }
}

void sql_thread::cancel()
{
  QMutexLocker locker(&m_mutex);
  while (!m_queue.empty()) m_queue.pop();
  if (isRunning()) m_cancel = true;
}

void sql_thread::push(std::shared_ptr<task> tsk)
{
  QMutexLocker locker(&m_mutex);
  m_queue.push(tsk);
  m_cancel = false;
  if (isRunning()) m_condition.wakeOne();
  else start(LowPriority);
}

void sql_thread::run()
{
  struct progress_impl : progress {
    sql_thread* m_thread;
    QString m_msg;
    size_t m_counter;
    QTime m_time;

    explicit progress_impl(sql_thread* thread_) : m_thread(thread_), m_counter(0)  { m_time.start(); }

    void init(const std::vector<std::string>& columns = std::vector<std::string>()) override
    {
      if (columns.empty()) return;
      m_thread->m_mdl->clear();
      m_thread->m_mdl->init(columns);
    }

    bool step(size_t counter = 0, const std::vector<std::string>& row = std::vector<std::string>()) override
    {
      if (m_thread->m_abort.load()) return false;
      if (m_thread->m_cancel.load()) return false;
      m_counter = counter;
      if (!row.empty()) m_thread->m_mdl->push_back(row);
      if (m_time.elapsed() > SignalInterval)
      {
        m_thread->m_mdl->update();
        m_thread->emit signal_process(QString("%1").arg(m_counter));
        m_time.restart();
      }
      return true;
    }
  }; // progress_impl

  forever
  {
    m_mutex.lock();
    while (m_queue.empty() && !m_abort.load())
    {
      emit signal_idle();
      m_condition.wait(&m_mutex);
      m_cancel = false;
    }
    std::shared_ptr<task> tsk;
    if (!m_queue.empty())
    {
      tsk = m_queue.front();
      m_queue.pop();
    }
    m_mutex.unlock();

    if (m_abort.load()) return;
    emit signal_start();
    progress_impl prg(this);
    try  { tsk->run(&prg); }
    catch (const std::exception& e)
    {
      prg.m_msg = QString::fromUtf8(e.what());
      if (prg.m_msg.isEmpty()) prg.m_msg = "error";
    }

    if (m_abort.load()) return;
    if (prg.m_msg.isEmpty() && m_cancel.load()) prg.m_msg = "canceled";
    if (prg.m_msg.isEmpty()) prg.m_msg = QString("%1").arg(prg.m_counter);
    m_mdl->update();
    emit signal_process(prg.m_msg);
  }
}
