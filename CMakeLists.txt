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
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets SerialPort Charts)

if(Qt6_VERSION VERSION_LESS ${QT_MIN_VERSION})
  message(FATAL_ERROR "Qt version ${Qt6_VERSION} found, but ${QT_MIN_VERSION} or higher is required")
endif()

qt_standard_project_setup()

file(GLOB SRC_FILES
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.h
)

file(GLOB QCUSTOMPLOT_FILES
    ${LIB_DIR}/qcustomplot/*.cpp
    ${LIB_DIR}/qcustomplot/*.h
)

file(GLOB QSCALE_FILES
    ${LIB_DIR}/QScale/*.cpp
    ${LIB_DIR}/QScale/*.h
)

qt_add_executable(OpenJTX_A9
    ${SRC_FILES}
    ${QSCALE_FILES}
    ${QCUSTOMPLOT_FILES}
)

target_include_directories(OpenJTX_A9 PRIVATE
    ${SRC_DIR}
    ${LIB_DIR}/QScale
    ${LIB_DIR}/qcustomplot
)

target_link_libraries(OpenJTX_A9 PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::SerialPort
  Qt6::Charts
)
