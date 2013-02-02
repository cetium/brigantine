// Andrew Naplavkov

#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <memory>
#include <QString>
#include <QVariant>
#include <vector>
#include "provider_ptr.h"
#include "layer_ptr.h"

class tree_item {
  QVariant m_var;

public:
  const tree_item* m_parent;
  std::vector<std::unique_ptr<tree_item>> m_children;

  tree_item(const tree_item* parent, provider_ptr pvd);
  tree_item(const tree_item* parent, layer_ptr lr);

  provider_ptr get_provider() const;
  layer_ptr get_layer() const;
  QString get_string() const;

  void check(size_t order);
  int position() const;
}; // tree_item

#endif // TREE_ITEM_H
