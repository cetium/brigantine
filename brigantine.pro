QT +=\
  core\
  gui

HEADERS +=\
  connection.h\
  connection_link.h\
  dialog_create.h\
  dialog_drop.h\
  dialog_insert.h\
  dialog_oci.h\
  dialog_odbc.h\
  frame.h\
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
  task_exec.h\
  task_insert.h\
  task_mbr.h\
  tree_item.h\
  tree_model.h\
  tree_view.h\
  utilities.h

SOURCES +=\
  connection.cpp\
  connection_link.cpp\
  dialog_create.cpp\
  dialog_drop.cpp\
  dialog_insert.cpp\
  dialog_oci.cpp\
  dialog_odbc.cpp\
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
  task_exec.cpp\
  task_insert.cpp\
  task_mbr.cpp\
  tree_item.cpp\
  tree_model.cpp\
  tree_view.cpp\
  utilities.cpp

FORMS +=\
  dialog_create.ui\
  dialog_drop.ui\
  dialog_oci.ui\
  dialog_odbc.ui

RESOURCES +=\
  resource.qrc

INCLUDEPATH +=\
  ../boost_1_49_0/\
  ../include/oci/\
  ../include/

Release:LIBS +=\
  ../boost_1_49_0/stage/lib/libboost_date_time-vc100-mt-1_49.lib\
  ../boost_1_49_0/stage/lib/libboost_thread-vc100-mt-1_49.lib

Debug:LIBS +=\
  ../boost_1_49_0/stage/lib/libboost_date_time-vc100-mt-gd-1_49.lib\
  ../boost_1_49_0/stage/lib/libboost_thread-vc100-mt-gd-1_49.lib
