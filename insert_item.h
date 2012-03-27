// Andrew Naplavkov

#ifndef INSERT_ITEM_H
#define INSERT_ITEM_H

#include <string>

struct insert_item {
  int level, epsg;
  std::string column_from, column_to;

  insert_item() : level(-1), epsg(-1)  {}
}; // insert_item

#endif // INSERT_ITEM_H
