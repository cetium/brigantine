// Andrew Naplavkov

#include <QAtomicInt>
#include <QDateTime>
#include <QEventLoop>
#include <QMutexLocker>
#include <stdexcept>
#include "task.h"

static QAtomicInt s_id = 1;

task::task()
  : m_id(s_id.fetchAndAddOrdered(1)), m_start(QDateTime::currentMSecsSinceEpoch()), m_finish(0), m_st(Waiting)
{}

int task::get_id() const
{
  return m_id;
}

QString task::get_status()
{
  QMutexLocker lck(&m_mtx);
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

bool task::is_canceling()
{
  QMutexLocker lck(&m_mtx);
  return m_st == Canceling;
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
  QMutexLocker lck(&m_mtx);
  return is_finished_impl();
}

qint64 task::get_finish()
{
  QMutexLocker lck(&m_mtx);
  if (is_finished_impl()) return m_finish;
  else return QDateTime::currentMSecsSinceEpoch();
}

qint64 task::get_milliseconds()
{
  return get_finish() - m_start;
}

QString task::get_message()
{
  QMutexLocker lck(&m_mtx);
  return m_msg;
}

frame task::get_frame()
{
  QMutexLocker lck(&m_mtx);
  return m_fr;
}

void task::set_frame(const frame& fr)
{
  QMutexLocker lck(&m_mtx);
  m_fr = fr;
}

void task::on_cancel()
{
  QMutexLocker lck(&m_mtx);
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
  QMutexLocker lck(&m_mtx);
  if (!msg.isEmpty()) m_msg = msg;
  if (m_st == Canceling) throw std::runtime_error("");
}

void task::run()
{
  try
  {
    {
      QMutexLocker lck(&m_mtx);
      if (m_st != Canceling) m_st = Running;
    }
    progress(QString());
    run_impl();
    {
      QMutexLocker lck(&m_mtx);
      m_finish = QDateTime::currentMSecsSinceEpoch();
      m_st = Complete;
    }
  }
  catch (const std::exception& e)
  {
    QMutexLocker lck(&m_mtx);
    m_finish = QDateTime::currentMSecsSinceEpoch();
    if (m_st == Canceling) m_st = Canceled;
    else m_st = Failed;
    QString msg = QString::fromUtf8(e.what());
    if (!msg.isEmpty()) m_msg = msg;
  }

  emit signal_finished();
}
