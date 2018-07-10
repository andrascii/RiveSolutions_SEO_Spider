cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)
project(${VersionProjectName})

set(VersionHeaders include)

set(MAJOR 1)
set(MINOR 0)
set(MAINTENANCE 4)


# targer files path
set(versionHeaderPath ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(installerConfigPath ${CMAKE_CURRENT_SOURCE_DIR}/../seospiderinstaller/packages/com.rivesolutions.seospider/meta)

file(GLOB_RECURSE VersionHeadersList "include/*.h")

source_group("Headers" FILES ${VersionHeadersList})

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/SetVersion.cmake.in 
	${CMAKE_CURRENT_SOURCE_DIR}/SetVersion.cmake
	@ONLY
)

configure_msvc_runtime()
add_library(
	${VersionProjectName} 	
	${VersionHeadersList}
)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h)

add_custom_command(
	TARGET ${VersionProjectName} PRE_BUILD 
	COMMAND ${CMAKE_COMMAND} -P 
	${CMAKE_CURRENT_SOURCE_DIR}/SetVersion.cmake
)

include_directories(
	${VersionHeaders}
)