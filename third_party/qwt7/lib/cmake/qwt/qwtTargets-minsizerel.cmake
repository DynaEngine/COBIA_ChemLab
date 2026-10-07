#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "qwt::core" for configuration "MinSizeRel"
set_property(TARGET qwt::core APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(qwt::core PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/libqwtcore.dll.a"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/libqwtcore.dll"
  )

list(APPEND _cmake_import_check_targets qwt::core )
list(APPEND _cmake_import_check_files_for_qwt::core "${_IMPORT_PREFIX}/lib/libqwtcore.dll.a" "${_IMPORT_PREFIX}/bin/libqwtcore.dll" )

# Import target "qwt::plot" for configuration "MinSizeRel"
set_property(TARGET qwt::plot APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(qwt::plot PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/libqwtplot.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_MINSIZEREL "Qt6::Concurrent;Qt6::PrintSupport"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/libqwtplot.dll"
  )

list(APPEND _cmake_import_check_targets qwt::plot )
list(APPEND _cmake_import_check_files_for_qwt::plot "${_IMPORT_PREFIX}/lib/libqwtplot.dll.a" "${_IMPORT_PREFIX}/bin/libqwtplot.dll" )

# Import target "qwt::plot3d" for configuration "MinSizeRel"
set_property(TARGET qwt::plot3d APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(qwt::plot3d PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/libqwtplot3d.dll.a"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/libqwtplot3d.dll"
  )

list(APPEND _cmake_import_check_targets qwt::plot3d )
list(APPEND _cmake_import_check_files_for_qwt::plot3d "${_IMPORT_PREFIX}/lib/libqwtplot3d.dll.a" "${_IMPORT_PREFIX}/bin/libqwtplot3d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
