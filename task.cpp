// Andrew Naplavkov

#include <QAtomicInt>
#include <QEventLoop>
#include <stdexcept>
#include "task.h"

static QAtomicInt s_id = 1;

task::task()
  : m_id(s_id.fetchAndAddOrdered(1)), m_start(std::chrono::system_clock::now()), m_st(Waiting)
{}

int task::get_id() const
{
  return m_id;
}

QString task::get_status()
{
  QMutexLocker locker(&m_mtx);
  switch (m_st)
  {
  default: return QString();
  case Waiting: return QString("waiting");
  case Running: return QString("running");
  case Canceling: return QString("canceling");
  case Canceled: return QString("canceled");
  case Failed: return QString("failed");
  case Complete: return QString("complete");
  }
}

bool task::is_finished_impl() const
{
  switch (m_st)
  {
  default: return false;
  case Canceled:
  case Failed:
  case Complete: return true;
  }
}

bool task::is_finished()
{
  QMutexLocker locker(&m_mtx);
  return is_finished_impl();
}

std::chrono::system_clock::time_point task::get_finish()
{
  QMutexLocker locker(&m_mtx);
  if (is_finished_impl()) return m_finish;
  else return std::chrono::system_clock::now();
}

int task::get_milliseconds()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(get_finish() - m_start).count();
}

QString task::get_message()
{
  QMutexLocker locker(&m_mtx);
  return m_msg;
}

frame task::get_frame()
{
  QMutexLocker locker(&m_mtx);
  return m_fr;
}

void task::set_frame(const frame& fr)
{
  QMutexLocker locker(&m_mtx);
  m_fr = fr;
}

void task::on_cancel()
{
  QMutexLocker locker(&m_mtx);
  if (!is_finished_impl()) m_st = Canceling;
}

void task::on_cancel(int id)
{
  if (id == m_id) on_cancel();
}

void task::progress(QString msg)
{
  QEventLoop loop(this);
  loop.processEvents();
  QMutexLocker locker(&m_mtx);
  if (!msg.isEmpty()) m_msg = msg;
  if (m_st == Canceling) throw std::runtime_error("");
}

void task::run()
{
  try
  {
    {
      QMutexLocker locker(&m_mtx);
      if (m_st != Canceling) m_st = Running;
    }
    progress(QString());
    run_impl();
    {
      QMutexLocker locker(&m_mtx);
      m_st = Complete;
    }
  }
  catch (const std::exception& e)
  {
    QMutexLocker locker(&m_mtx);
    if (m_st == Canceling) m_st = Canceled;
    else m_st = Failed;
    QString msg = QString::fromUtf8(e.what());
    if (!msg.isEmpty()) m_msg = msg;
  }
  m_finish = std::chrono::system_clock::now();
  emit signal_finished();
}
