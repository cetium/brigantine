// Andrew Naplavkov

#include "rowset_model.h"

rowset_model::rowset_model(QObject* parent) : QAbstractItemModel(parent)
{}

int rowset_model::columnCount(const QModelIndex&) const
{
  return int(m_columns.size());
}

QVariant rowset_model::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (section >= int(m_columns.size()) || orientation != Qt::Horizontal ||role != Qt::DisplayRole) return QVariant();
  return QString::fromUtf8(m_columns[section].c_str());
}

QVariant rowset_model::data(const QModelIndex& index, int role) const
{
  if (role != Qt::DisplayRole) return QVariant();
  auto str(static_cast<const char*>(index.internalPointer()));
  return str? QString::fromUtf8(str): QString();
}

QModelIndex rowset_model::index(int row, int column, const QModelIndex&) const
{
  if (row < 0 || column < 0) return QModelIndex();
  const char* str(0);
  {
    if (row >= int(m_rows.size()) || column >= int(m_rows[row].size())) return QModelIndex();
    str = m_rows[row][column].c_str();
  }
  return createIndex(row, column, (void*)str);
}

int rowset_model::rowCount(const QModelIndex&) const
{
  return int(m_rows.size());
}
