include_guard(GLOBAL)

include(GNUInstallDirs)

function(openjtx_install_target target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "Unknown target passed to openjtx_install_target: ${target}")
  endif()

  install(TARGETS "${target}"
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT Runtime
  )

  if(WIN32 OR APPLE)
    qt_generate_deploy_app_script(
      TARGET "${target}"
      OUTPUT_SCRIPT openjtx_deploy_script
      NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(SCRIPT "${openjtx_deploy_script}" COMPONENT Runtime)
  endif()

  if(UNIX AND NOT APPLE)
    install(
      DIRECTORY "${PROJECT_SOURCE_DIR}/resources/icons/hicolor"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons"
      COMPONENT Runtime
    )
    install(
      FILES "${PROJECT_SOURCE_DIR}/resources/linux/OpenJTXA9.desktop"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
      COMPONENT Runtime
    )
  endif()
endfunction()
