// Andrew Naplavkov

#ifndef INSERT_MAP_H
#define INSERT_MAP_H

#include <string>
#include <vector>

struct insert_map_item {
  int level, epsg;
  std::string column_from, column_to;

  insert_map_item() : level(-1), epsg(-1)  {}
}; // insert_map_item

typedef std::vector<insert_map_item> insert_map;

#endif // INSERT_MAP_H
