#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "check" for configuration "Debug"
set_property(TARGET check APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(check PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "m;rt"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libcheck.a"
  )

list(APPEND _cmake_import_check_targets check )
list(APPEND _cmake_import_check_files_for_check "${_IMPORT_PREFIX}/lib/libcheck.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
