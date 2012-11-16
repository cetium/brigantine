// Andrew Naplavkov

#ifndef SQL_MODEL_H
#define SQL_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMutex>
#include <QString>
#include <QVariant>
#include <string>
#include <vector>

class sql_model : public QAbstractItemModel {
  Q_OBJECT

  QMutex m_mutex;
  std::vector<std::string> m_columns;
  std::vector<std::vector<std::string>> m_rows;

public:
  explicit sql_model(QObject* parent = 0);
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex&) const override  { return QModelIndex(); }
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  void clear();
  void init(const std::vector<std::string>& columns);
  void push_back(const std::vector<std::string>& row);
  void update()  { reset(); }
}; // sql_model

#endif // SQL_MODEL_H
