cmake_minimum_required(VERSION 3.25)

project(OpenJTX_A9 VERSION 1.0 LANGUAGES C CXX)

set(QT_MIN_VERSION 6.4)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets SerialPort)

if(Qt6_VERSION VERSION_LESS ${QT_MIN_VERSION})
    message(FATAL_ERROR "Qt version ${Qt6_VERSION} found, but ${QT_MIN_VERSION} or higher is required")
endif()

qt_standard_project_setup()

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

qt_add_executable(OpenJTX_A9 ${SRC_FILES})

target_link_libraries(OpenJTX_A9 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
)
