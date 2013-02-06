// Andrew Naplavkov

#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <brig/osm/layer.hpp>
#include <brig/proj/shared_pj.hpp>
#include <memory>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <vector>
#include "task.h"
#include "tree_item.h"

class tree_model : public QAbstractItemModel {
  Q_OBJECT

  tree_item m_root;
  size_t m_order;

  void connect_to(provider_ptr pvd);
  void emit_layers();

public slots:
  void on_refresh(provider_ptr pvd);

signals:
  void signal_layers(std::vector<layer_ptr> lrs);
  void signal_disconnect(provider_ptr pvd);

public:
  explicit tree_model(QObject* parent = 0);
  int columnCount(const QModelIndex&) const override  { return 1; }
  QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& idx) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& idx) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool setData(const QModelIndex& idx, const QVariant& value, int role = Qt::EditRole) override;

  void connect_gdal(QString file);
  void connect_mysql(QString host, int port, QString db, QString usr, QString pwd);
  void connect_odbc(QString dsn);
  void connect_ogr(QString file, QString drv = QString(), QString fitted_id = QString());
  void connect_oracle(QString host, int port, QString db, QString usr, QString pwd);
  void connect_osm(std::shared_ptr<brig::osm::layer> lr);
  void connect_postgres(QString host, int port, QString db, QString usr, QString pwd);
  void connect_sqlite(QString file, bool init = false);
  void disconnect(const QModelIndex& idx);
  void refresh(const QModelIndex& idx);

  bool is_provider(const QModelIndex& idx) const;
  provider_ptr get_provider(const QModelIndex& idx) const;
  bool is_layer(const QModelIndex& idx) const;
  layer_ptr get_layer(const QModelIndex& idx) const;
  bool has_checked() const;
  void push_back_checked(std::vector<layer_ptr>& lrs) const;
}; // tree_model

#endif // TREE_MODEL_H
