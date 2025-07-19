#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "civetweb::civetweb" for configuration ""
set_property(TARGET civetweb::civetweb APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(civetweb::civetweb PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcivetweb.a"
  )

list(APPEND _cmake_import_check_targets civetweb::civetweb )
list(APPEND _cmake_import_check_files_for_civetweb::civetweb "${_IMPORT_PREFIX}/lib/libcivetweb.a" )

# Import target "civetweb::server" for configuration ""
set_property(TARGET civetweb::server APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(civetweb::server PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/civetweb"
  )

list(APPEND _cmake_import_check_targets civetweb::server )
list(APPEND _cmake_import_check_files_for_civetweb::server "${_IMPORT_PREFIX}/bin/civetweb" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
