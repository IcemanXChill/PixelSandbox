#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFGUI::SFGUI" for configuration "Debug"
set_property(TARGET SFGUI::SFGUI APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFGUI::SFGUI PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/SFGUI-d.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/SFGUI-d.dll"
  )

list(APPEND _cmake_import_check_targets SFGUI::SFGUI )
list(APPEND _cmake_import_check_files_for_SFGUI::SFGUI "${_IMPORT_PREFIX}/debug/lib/SFGUI-d.lib" "${_IMPORT_PREFIX}/debug/bin/SFGUI-d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
