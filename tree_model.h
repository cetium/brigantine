// Andrew Naplavkov

#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <memory>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <vector>
#include "layer_ptr.h"
#include "provider_ptr.h"
#include "tree_item.h"

class tree_model : public QAbstractItemModel {
  Q_OBJECT

  tree_item m_root;

  void connect_to(provider_ptr pvd);
  void emit_layers();

public slots:
  void on_connected(provider_ptr pvd, std::vector<layer_ptr> lrs);

signals:
  void signal_layers(std::vector<layer_ptr> lrs);
  void signal_disconnect(provider_ptr pvd);

public:
  explicit tree_model(QObject* parent);
  int columnCount(const QModelIndex&) const override  { return 1; }
  QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& idx) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& idx) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool setData(const QModelIndex& idx, const QVariant& value, int role = Qt::EditRole) override;

  void disconnect(const QModelIndex& idx);
  bool is_provider(const QModelIndex& idx) const;
  provider_ptr get_provider(const QModelIndex& idx) const;
  bool is_layer(const QModelIndex& idx) const;
  layer_ptr get_layer(const QModelIndex& idx) const;
  bool has_checked() const;
  void push_back_checked(std::vector<layer_ptr>& lrs) const;
}; // tree_model

#endif // TREE_MODEL_H
