cmake_minimum_required(VERSION 3.25)

project(
  OpenJTX_A9
  DESCRIPTION "Alternative open-source software for multimeter with bluetooth or serial connection"
  LANGUAGES C CXX ASM
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS
  Quick
  SerialPort
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(OpenJTX_A9
  src/main.cpp
  src/SerialManager.cpp
)

qt_add_qml_module(OpenJTX_A9
  URI OpenJTX
  VERSION 1.0
  QML_FILES
  OpenJTX_A9/OpenJTX_A9Content/App.qml
)

target_link_libraries(OpenJTX_A9 PRIVATE
  Qt6::Quick
  Qt6::SerialPort
)
