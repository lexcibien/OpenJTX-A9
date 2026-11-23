qt_add_executable(OpenJTX_A9
  WIN32
  MACOSX_BUNDLE
  ComboBoxHelper.h
  PortComboBox.h
  PowerSupplyTab.cpp
  PowerSupplyTab.h
  SerialManager.cpp
  SerialManager.h
  main.cpp
  mainWindow.cpp
  mainWindow.h
  tabs/ChargingUSBTab.h
  tabs/CurveHistoryTab.h
  tabs/DiodeVoltageTab.h
  tabs/VoltageTab.h
  types.h
  ${PROJECT_SOURCE_DIR}/resources/OpenJTXA9.qrc
  ${PROJECT_SOURCE_DIR}/ui/MainWindow.ui
  ${PROJECT_SOURCE_DIR}/ui/WarnDialog.ui
)

set_target_properties(OpenJTX_A9 PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/ui"
  CXX_SCAN_FOR_MODULES OFF
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

target_include_directories(OpenJTX_A9 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenJTX_A9
  PRIVATE
    OpenJTX::QScale
    Qt6::Charts
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
)

include(OpenJTXPlatform)
openjtx_configure_platform_assets(OpenJTX_A9)

if(WIN32 AND OPENJTX_DEPLOY_WINDOWS_RUNTIME)
  if(TARGET Qt6::windeployqt)
    set(windeployqt "$<TARGET_FILE:Qt6::windeployqt>")
  else()
    set(windeployqt "$<TARGET_FILE_DIR:Qt6::qmake>/windeployqt.exe")
  endif()

  add_custom_target(OpenJTX_A9_runtime ALL
    COMMAND "${windeployqt}"
      --dir "$<TARGET_FILE_DIR:OpenJTX_A9>"
      "$<TARGET_FILE:OpenJTX_A9>"
    DEPENDS OpenJTX_A9
    COMMENT "Deploying the Qt runtime for OpenJTX_A9"
    VERBATIM
  )
endif()
