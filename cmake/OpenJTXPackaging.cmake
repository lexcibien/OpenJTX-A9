include_guard(GLOBAL)

function(openjtx_configure_packaging)
  set(CPACK_PACKAGE_NAME "OpenJTX-A9")
  set(CPACK_PACKAGE_VENDOR "OpenJTX-A9 contributors")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Communication software for the JTXTools A9 multimeter"
  )
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
  set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
  set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
  )

  if(WIN32 OR APPLE)
    set(CPACK_GENERATOR ZIP)
  else()
    set(CPACK_GENERATOR TGZ)
  endif()

  include(CPack)
endfunction()
