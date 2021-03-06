cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)

project(QtXlsxWriter)
add_definitions(-DQT_BUILD_XLSX_LIB)
set(BUILD_SHARED_LIBS TRUE)
set(CMAKE_AUTOMOC ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

file(
	GLOB
	QtXlsxWriter_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/xlsx/*.cpp
	${CMAKE_CURRENT_BINARY_DIR}/QtXlsxWriterTest_automoc.cpp
)

find_package(Qt5 5.5 REQUIRED Core Gui Test)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/xlsx/
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
	${Qt5Gui_PRIVATE_INCLUDE_DIRS} )

SET(QtXlsxWriter_INCLUDE_FILES
  src/xlsx/xlsxabstractooxmlfile.h
  src/xlsx/xlsxabstractsheet.h
  src/xlsx/xlsxcell.h
  src/xlsx/xlsxcellformula.h
  src/xlsx/xlsxcellrange.h
  src/xlsx/xlsxcellreference.h
  src/xlsx/xlsxchart.h
  src/xlsx/xlsxchartsheet.h
  src/xlsx/xlsxconditionalformatting.h
  src/xlsx/xlsxdatavalidation.h
  src/xlsx/xlsxdocument.h
  src/xlsx/xlsxformat.h
  src/xlsx/xlsxglobal.h
  src/xlsx/xlsxrichstring.h
  src/xlsx/xlsxworkbook.h
  src/xlsx/xlsxworksheet.h
)

if(MSVC)

	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/permissive-
		/Zi
	)

endif()

configure_msvc_runtime()
add_library(QtXlsxWriter SHARED ${QtXlsxWriter_SOURCE_FILES} ${QtXlsxWriter_INCLUDE_FILES})

# automatically add C++11 support with GCC
if(NOT MSVC)
    target_compile_features(QtXlsxWriter PRIVATE cxx_range_for)
endif()

if (UNIX)
    set_target_properties(QtXlsxWriter PROPERTIES DEBUG_POSTFIX "d" RUNTIME_OUTPUT_DIRECTORY "../seospider/")
else()
    set_target_properties(QtXlsxWriter PROPERTIES DEBUG_POSTFIX "d" RUNTIME_OUTPUT_DIRECTORY "../seospider/" LINK_FLAGS /DEBUG)
endif()

target_link_libraries(QtXlsxWriter ${Qt5Core_LIBRARIES})
target_link_libraries(QtXlsxWriter ${Qt5Gui_LIBRARIES})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_custom_command(TARGET QtXlsxWriter POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        -E copy_directory $<CONFIGURATION> ${CMAKE_CURRENT_BINARY_DIR}/examples/xlsx/$<CONFIGURATION>)
	add_subdirectory(examples/xlsx)
endif()

##
#
# QtxlsxwriterVersion.cmake creation
#
##
set(QtXlsxWriter_CONFIG_PATH ${CMAKE_INSTALL_PREFIX})
configure_file(QtXlsxWriterConfig.cmake.in QtXlsxWriterConfig.cmake @ONLY)

#####
#
# Installation configuration
#
#####
INSTALL(TARGETS QtXlsxWriter
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

INSTALL(FILES  ${CMAKE_CURRENT_BINARY_DIR}/QtXlsxWriterConfig.cmake DESTINATION lib/cmake/${PROJECT_NAME})
INSTALL(FILES ${QtXlsxWriter_INCLUDE_FILES} DESTINATION include)

