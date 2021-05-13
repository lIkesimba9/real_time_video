#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "estimator" for configuration ""
set_property(TARGET estimator APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(estimator PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/estimator/libestimator.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS estimator )
list(APPEND _IMPORT_CHECK_FILES_FOR_estimator "${_IMPORT_PREFIX}/estimator/libestimator.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
