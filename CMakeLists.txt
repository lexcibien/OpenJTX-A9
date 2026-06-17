cmake_minimum_required(VERSION 3.25)

project(
    OpenJTX_A9
    DESCRIPTION "Alternative open-source software for multimeter with bluetooth or serial connection"
    LANGUAGES C CXX ASM
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    SerialPort
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(OpenJTX_A9
    src/main.cpp
    src/SerialManager.cpp
    src/mainWindow.h
    src/mainWindow.cpp

    ui/MainWindow.ui
)

target_link_libraries(OpenJTX_A9 PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
)
