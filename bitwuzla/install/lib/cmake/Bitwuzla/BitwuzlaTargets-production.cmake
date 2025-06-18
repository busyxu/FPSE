#----------------------------------------------------------------
# Generated CMake target import file for configuration "Production".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Bitwuzla::bitwuzla" for configuration "Production"
set_property(TARGET Bitwuzla::bitwuzla APPEND PROPERTY IMPORTED_CONFIGURATIONS PRODUCTION)
set_target_properties(Bitwuzla::bitwuzla PROPERTIES
  IMPORTED_LOCATION_PRODUCTION "${_IMPORT_PREFIX}/lib/libbitwuzla.so"
  IMPORTED_SONAME_PRODUCTION "libbitwuzla.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS Bitwuzla::bitwuzla )
list(APPEND _IMPORT_CHECK_FILES_FOR_Bitwuzla::bitwuzla "${_IMPORT_PREFIX}/lib/libbitwuzla.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
