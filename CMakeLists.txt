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
set(RESOURCE_FILES resources/OpenJTXA9.qrc)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets SerialPort Charts)

if(Qt6_VERSION VERSION_LESS ${QT_MIN_VERSION})
  message(FATAL_ERROR "Qt version ${Qt6_VERSION} found, but ${QT_MIN_VERSION} or higher is required")
endif()

qt_standard_project_setup()

file(GLOB SRC_FILES
  ${SRC_DIR}/*.cpp
  ${SRC_DIR}/*.h
)

file(GLOB QSCALE_FILES
  ${LIB_DIR}/QScale/*.cpp
  ${LIB_DIR}/QScale/*.h
)

qt_add_executable(OpenJTX_A9
  MACOSX_BUNDLE
  ${RESOURCE_FILES}
  ${SRC_FILES}
  ${QSCALE_FILES}
)

if(WIN32)
  target_sources(OpenJTX_A9 PRIVATE
    resources/windows/OpenJTXA9.rc
  )
endif()

if(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE OpenJTXA9)

  target_sources(OpenJTX_A9 PRIVATE
    resources/icons/OpenJTXA9.icns
  )

  set_source_files_properties(
    resources/icons/OpenJTXA9.icns
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
  )

endif()

target_include_directories(OpenJTX_A9
  PRIVATE
  ${SRC_DIR}

  SYSTEM PRIVATE
  ${LIB_DIR}/QScale
)

target_link_libraries(OpenJTX_A9 PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::SerialPort
  Qt6::Charts
)

install(TARGETS OpenJTX_A9
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
  TARGET OpenJTX_A9
  OUTPUT_SCRIPT deploy_script
  NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

if(UNIX AND NOT APPLE)
  install(FILES
    icons/hicolor/scalable/apps/OpenJTXA9.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
  )

  install(FILES
    resources/linux/OpenJTX_A9.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
  )
endif()
