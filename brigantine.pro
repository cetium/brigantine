QT +=\
  core\
  gui

HEADERS +=\
  connection.h\
  connection_link.h\
  counter_clockwise.h\
  dialog_connect.h\
  dialog_do.h\
  dialog_insert.h\
  dialog_odbc.h\
  dialog_shape.h\
  frame.h\
  global.h\
  insert_item.h\
  layer.h\
  layer_geometry.h\
  layer_link.h\
  layer_raster.h\
  main_window.h\
  map_thread.h\
  map_view.h\
  progress.h\
  reproject.h\
  sql_model.h\
  sql_thread.h\
  sql_view.h\
  task.h\
  task_attributes.h\
  task_create.h\
  task_drop.h\
  task_exec.h\
  task_insert.h\
  task_mbr.h\
  task_proj.h\
  tree_item.h\
  tree_model.h\
  tree_view.h\
  utilities.h

SOURCES +=\
  connection.cpp\
  connection_link.cpp\
  counter_clockwise.cpp\
  dialog_connect.cpp\
  dialog_do.cpp\
  dialog_insert.cpp\
  dialog_odbc.cpp\
  dialog_shape.cpp\
  frame.cpp\
  layer.cpp\
  layer_geometry.cpp\
  layer_link.cpp\
  layer_raster.cpp\
  main.cpp\
  main_window.cpp\
  map_thread.cpp\
  map_view.cpp\
  reproject.cpp\
  sql_model.cpp\
  sql_thread.cpp\
  sql_view.cpp\
  task_attributes.cpp\
  task_create.cpp\
  task_drop.cpp\
  task_exec.cpp\
  task_insert.cpp\
  task_mbr.cpp\
  task_proj.cpp\
  tree_item.cpp\
  tree_model.cpp\
  tree_view.cpp\
  utilities.cpp

FORMS +=\
  dialog_connect.ui\
  dialog_do.ui\
  dialog_odbc.ui

RESOURCES +=\
  resource.qrc

QMAKE_CXXFLAGS +=\
  -fno-tree-vectorize\
  -std=c++0x

# boost, brig, ...
win32:INCLUDEPATH +=\
  ../include/\
  ../include/libpq/\
  ../include/mysql\
  ../include/oci/

unix:INCLUDEPATH +=\
  ../include/\
  /usr/include/postgresql/\
  /usr/include/mysql\
  ../include/oci/

unix:QMAKE_RPATHDIR +=\
  '${ORIGIN}'
