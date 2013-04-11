// Andrew Naplavkov

#ifndef ROWSET_MODEL_H
#define ROWSET_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <string>
#include <vector>

class rowset_model : public QAbstractItemModel {
public:
  std::vector<std::string> m_columns;
  std::vector<std::vector<std::string>> m_rows;

  explicit rowset_model(QObject* parent = 0);
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex&) const override  { return QModelIndex(); }
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
}; // rowset_model

#endif // ROWSET_MODEL_H
