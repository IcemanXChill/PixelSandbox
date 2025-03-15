#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFGUI::SFGUI" for configuration "Release"
set_property(TARGET SFGUI::SFGUI APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFGUI::SFGUI PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/SFGUI.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/SFGUI.dll"
  )

list(APPEND _cmake_import_check_targets SFGUI::SFGUI )
list(APPEND _cmake_import_check_files_for_SFGUI::SFGUI "${_IMPORT_PREFIX}/lib/SFGUI.lib" "${_IMPORT_PREFIX}/bin/SFGUI.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
