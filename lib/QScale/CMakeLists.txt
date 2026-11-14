qt_add_library(OpenJTX_QScale STATIC
  qscale.cpp
  qscale.h
)

add_library(OpenJTX::QScale ALIAS OpenJTX_QScale)

set_target_properties(OpenJTX_QScale PROPERTIES
  AUTOMOC ON
  CXX_SCAN_FOR_MODULES OFF
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(OpenJTX_QScale
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(OpenJTX_QScale
  PUBLIC
    Qt6::Widgets
  PRIVATE
    Qt6::Gui
)
