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
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex&) const  { return QModelIndex(); }
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

  void clear();
  void init(const std::vector<std::string>& columns);
  void push_back(const std::vector<std::string>& row);
  void update();
}; // sql_model

#endif // SQL_MODEL_H
