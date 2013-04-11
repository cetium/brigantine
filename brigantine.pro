QT +=\
  core\
  gui

greaterThan(QT_MAJOR_VERSION, 4):QT +=\
  widgets

HEADERS +=\
  clickable_label.h\
  counter_clockwise.h\
  dialog_connect.h\
  dialog_create.h\
  dialog_insert.h\
  dialog_odbc.h\
  frame.h\
  global.h\
  insert_item.h\
  layer.h\
  layer_geometry.h\
  layer_ptr.h\
  layer_raster.h\
  main_window.h\
  map_view.h\
  provider.h\
  provider_ptr.h\
  reproject.h\
  rowset_model.h\
  sql_view.h\
  task.h\
  task_attributes.h\
  task_connect.h\
  task_create.h\
  task_drop.h\
  task_extent.h\
  task_fetch.h\
  task_insert.h\
  task_proj.h\
  task_rendering.h\
  task_scale.h\
  task_scheduler.h\
  task_tables.h\
  tree_item.h\
  tree_model.h\
  tree_view.h\
  utilities.h

SOURCES +=\
  clickable_label.cpp\
  counter_clockwise.cpp\
  dialog_connect.cpp\
  dialog_create.cpp\
  dialog_insert.cpp\
  dialog_odbc.cpp\
  frame.cpp\
  layer.cpp\
  layer_geometry.cpp\
  layer_ptr.cpp\
  layer_raster.cpp\
  main.cpp\
  main_window.cpp\
  map_view.cpp\
  provider.cpp\
  provider_ptr.cpp\
  reproject.cpp\
  rowset_model.cpp\
  sql_view.cpp\
  task.cpp\
  task_attributes.cpp\
  task_connect.cpp\
  task_create.cpp\
  task_drop.cpp\
  task_extent.cpp\
  task_fetch.cpp\
  task_insert.cpp\
  task_proj.cpp\
  task_rendering.cpp\
  task_scale.cpp\
  task_scheduler.cpp\
  task_tables.cpp\
  tree_item.cpp\
  tree_model.cpp\
  tree_view.cpp\
  utilities.cpp

FORMS +=\
  dialog_connect.ui\
  dialog_create.ui\
  dialog_odbc.ui

RESOURCES +=\
  resource.qrc

# boost, brig, ...
INCLUDEPATH +=\
  ../include/\
  ../include/oci/

windows:INCLUDEPATH +=\
  ../include/gdal/\
  ../include/mysql/\
  ../include/postgresql/

unix:INCLUDEPATH +=\
  /usr/include/gdal/\
  /usr/include/mysql/\
  /usr/include/postgresql/

unix:QMAKE_RPATHDIR +=\
  '${ORIGIN}'

*-g++*:QMAKE_CXXFLAGS +=\
  -fno-tree-vectorize\
  -std=c++0x

windows:DEFINES +=\
  NOMINMAX
