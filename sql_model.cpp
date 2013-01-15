// Andrew Naplavkov

#include "sql_model.h"
#include "utilities.h"

sql_model::sql_model(QObject* parent) : QAbstractItemModel(parent)  {}

int sql_model::columnCount(const QModelIndex&) const
{
  QMutexLocker locker((QMutex*)&m_mutex);
  return int(m_columns.size());
}

QVariant sql_model::headerData(int section, Qt::Orientation orientation, int role) const
{
  QMutexLocker locker((QMutex*)&m_mutex);
  if (section >= int(m_columns.size()) || orientation != Qt::Horizontal ||role != Qt::DisplayRole) return QVariant();
  return QString::fromUtf8(m_columns[section].c_str());
}

QVariant sql_model::data(const QModelIndex& index, int role) const
{
  if (role != Qt::DisplayRole) return QVariant();
  auto str(static_cast<const char*>(index.internalPointer()));
  return str? QString::fromUtf8(str): QString();
}

QModelIndex sql_model::index(int row, int column, const QModelIndex&) const
{
  if (row < 0 || column < 0) return QModelIndex();
  const char* str(0);
  {
    QMutexLocker locker((QMutex*)&m_mutex);
    if (row >= int(m_rows.size()) || column >= int(m_rows[row].size())) return QModelIndex();
    str = m_rows[row][column].c_str();
  }
  return createIndex(row, column, (void*)str);
}

int sql_model::rowCount(const QModelIndex&) const
{
  QMutexLocker locker((QMutex*)&m_mutex);
  return int(m_rows.size());
}

void sql_model::init(const std::vector<std::string>& columns)
{
  QMutexLocker locker((QMutex*)&m_mutex);
  m_columns = columns;
}

void sql_model::push_back(const std::vector<std::string>& row)
{
  QMutexLocker locker((QMutex*)&m_mutex);
  m_rows.push_back(row);
}

void sql_model::clear()
{
  beginResetModel();
  {
    QMutexLocker locker((QMutex*)&m_mutex);
    m_columns.clear();
    m_rows.clear();
  }
  endResetModel();
}

void sql_model::update()
{
  beginResetModel();
  endResetModel();
}
