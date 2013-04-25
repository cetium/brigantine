// Andrew Naplavkov

#include <algorithm>
#include <brig/global.hpp>
#include <chrono>
#include <iterator>
#include <QString>
#include <QThread>
#include <QThreadPool>
#include <utility>
#include "task_model.h"
#include "utilities.h"

task_model::task_model(QObject* parent) : QAbstractItemModel(parent), m_activity(0)
{
  int threads(QThread::idealThreadCount());
  if (threads <= 0 || threads > int(brig::PoolSize)) threads = int(brig::PoolSize);
  QThreadPool::globalInstance()->setMaxThreadCount(threads);
  startTimer(RenderingInterval);
}

task_model::~task_model()
{
  emit signal_cancel_all();
}

int task_model::columnCount(const QModelIndex&) const
{
  return 5;
}

QVariant task_model::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
  switch (section)
  {
  case 0: return QString("id");
  case 1: return QString("task");
  case 2: return QString("status");
  case 3: return QString("message");
  case 4: return QString("time");
  }
  return QVariant();
}

QModelIndex task_model::index(int row, int column, const QModelIndex&) const
{
  if (row < 0 || row >= int(m_tsks.size()) || column < 0 || column >= 5) return QModelIndex();
  return createIndex(row, column, (void*)m_tsks[row]->get_id());
}

std::shared_ptr<task> task_model::get_task(const QModelIndex& index) const
{
  if (!index.isValid() || index.row() >= int(m_tsks.size())) return std::shared_ptr<task>();
  std::shared_ptr<task> tsk = m_tsks[index.row()];
  if (tsk->get_id() != int(index.internalPointer())) return std::shared_ptr<task>();
  return tsk;
}

QVariant task_model::data(const QModelIndex& index, int role) const
{
  if (role != Qt::DisplayRole) return QVariant();
  auto tsk = get_task(index);
  if (!tsk) return QVariant();

  switch (index.column())
  {
  case 0: return QString("%1").arg(tsk->get_id());
  case 1: return tsk->get_string();
  case 2: return tsk->get_status();
  case 3: return tsk->get_message();
  case 4:
    {
    QString sec;
    sec.setNum(double(tsk->get_milliseconds()) / 1000., 'f', 1);
    return sec;
    }
  }
  return QVariant();
}

int task_model::rowCount(const QModelIndex&) const
{
  return int(m_tsks.size());
}

void task_model::run(std::shared_ptr<task> tsk)
{
  class task_ptr : public QRunnable {
    std::shared_ptr<task> m_tsk;
  public:
    explicit task_ptr(std::shared_ptr<task> tsk) : m_tsk(tsk)  {}
    void run() override  { m_tsk->run(); }
  }; // task_ptr

  connect(this, SIGNAL(signal_cancel_all()), tsk.get(), SLOT(on_cancel()));
  connect(this, SIGNAL(signal_cancel(int)), tsk.get(), SLOT(on_cancel(int)));
  connect(tsk.get(), SIGNAL(signal_finished()), this, SLOT(on_finished()));

  int pos(int(m_tsks.size()) - 1);
  beginInsertRows(QModelIndex(), pos, pos);
  m_tsks.push_back(tsk);
  ++m_activity;
  endInsertRows();

  QThreadPool::globalInstance()->start(new task_ptr(tsk), tsk->get_priority());
  if (m_activity == 1) emit signal_progress();
}

void task_model::vacuum()
{
  std::vector<std::pair<std::chrono::system_clock::time_point, int>> time_to_order;
  for (size_t i(0); i < m_tsks.size(); ++i)
    time_to_order.push_back(std::make_pair(m_tsks[i]->get_finish(), i));
  std::sort(std::begin(time_to_order), std::end(time_to_order));
  for (size_t i(0); i < brig::PageSize; ++i)
    m_tsks[time_to_order[i].second].reset();

  beginResetModel();
  auto end = std::remove_if(std::begin(m_tsks), std::end(m_tsks), [](const std::shared_ptr<task>& tsk){ return !bool(tsk); });
  m_tsks.erase(end, std::end(m_tsks));
  endResetModel();
}

void task_model::on_finished()
{
  --m_activity;
  if (m_activity == 0) emit signal_idle();
}

void task_model::timerEvent(QTimerEvent*)
{
  dataChanged(QModelIndex(), QModelIndex());
  if ((m_tsks.size() - m_activity) > 2 * brig::PageSize) emit signal_need_vacuum();
}
