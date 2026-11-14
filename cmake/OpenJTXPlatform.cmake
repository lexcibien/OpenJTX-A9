include_guard(GLOBAL)

function(openjtx_configure_platform_assets target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "Unknown target passed to openjtx_configure_platform_assets: ${target}")
  endif()

  if(WIN32)
    target_sources("${target}" PRIVATE
      "${PROJECT_SOURCE_DIR}/resources/windows/OpenJTXA9.rc"
    )
    set_target_properties("${target}" PROPERTIES WIN32_EXECUTABLE ON)
  elseif(APPLE)
    set(openjtx_macos_icon "${PROJECT_SOURCE_DIR}/resources/icons/OpenJTXA9.icns")

    target_sources("${target}" PRIVATE "${openjtx_macos_icon}")
    set_source_files_properties("${openjtx_macos_icon}" PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
    )
    set_target_properties("${target}" PROPERTIES
      MACOSX_BUNDLE_ICON_FILE OpenJTXA9
    )
  endif()
endfunction()
