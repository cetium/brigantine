QT +=\
  core\
  gui

HEADERS +=\
  connection.h\
  connection_link.h\
  dialog_oci.h\
  dialog_odbc.h\
  frame.h\
  layer.h\
  layer_geometry.h\
  layer_link.h\
  layer_raster.h\
  main_window.h\
  map_thread.h\
  map_view.h\
  progress.h\
  sql_model.h\
  sql_thread.h\
  sql_view.h\
  task.h\
  task_attributes.h\
  task_exec.h\
  task_mbr.h\
  tree_item.h\
  tree_model.h\
  tree_view.h\
  utilities.h

SOURCES +=\
  connection.cpp\
  connection_link.cpp\
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
  sql_model.cpp\
  sql_thread.cpp\
  sql_view.cpp\
  task_attributes.cpp\
  task_exec.cpp\
  task_mbr.cpp\
  tree_item.cpp\
  tree_model.cpp\
  tree_view.cpp\
  utilities.cpp

FORMS +=\
  res/oci.ui\
  res/odbc.ui

INCLUDEPATH +=\
  D:/projects/boost_1_49_0/\
  D:/projects/include/oci/\
  D:/projects/include/

RESOURCES +=\
  res/resource.qrc

Release:LIBS +=\
  D:/projects/boost_1_49_0/stage/lib_64bit/libboost_date_time-vc100-mt-1_49.lib\
  D:/projects/boost_1_49_0/stage/lib_64bit/libboost_thread-vc100-mt-1_49.lib

Debug:LIBS +=\
  D:/projects/boost_1_49_0/stage/lib_64bit/libboost_date_time-vc100-mt-gd-1_49.lib\
  D:/projects/boost_1_49_0/stage/lib_64bit/libboost_thread-vc100-mt-gd-1_49.lib
