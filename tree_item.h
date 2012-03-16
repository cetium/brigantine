// Andrew Naplavkov

#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <memory>
#include <QString>
#include <QVariant>
#include <vector>
#include "connection_link.h"
#include "layer_link.h"

class tree_item {
  QVariant m_var;

public:
  const tree_item* m_parent;
  std::vector<std::unique_ptr<tree_item>> m_children;

  tree_item(const tree_item* parent, connection_link dbc);
  tree_item(const tree_item* parent, layer_link lr);

  connection_link get_connection() const;
  layer_link get_layer() const;
  QString get_string() const;

  void check(size_t order);
  int position() const;
}; // tree_item

#endif // TREE_ITEM_H
