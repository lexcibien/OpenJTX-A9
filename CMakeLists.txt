# Program for JTXTools A9 multimeter

cmake_minimum_required(VERSION 4.3)

cmake_policy(SET CMP0207 NEW) # Normalize runtime dependency paths before matching.
set(CMAKE_POLICY_DEFAULT_CMP0207 NEW)

project(OpenJTX_A9 VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(GNUInstallDirs)

set(OPENJTX_QT_MIN_VERSION 6.4)

find_package(Qt6 ${OPENJTX_QT_MIN_VERSION} CONFIG REQUIRED
  COMPONENTS
    Charts
    Core
    Gui
    SerialPort
    Widgets
)

qt_standard_project_setup()

add_subdirectory(lib/QScale)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

include(OpenJTXInstall)
openjtx_install_target(OpenJTX_A9)

include(OpenJTXPackaging)
openjtx_configure_packaging()
