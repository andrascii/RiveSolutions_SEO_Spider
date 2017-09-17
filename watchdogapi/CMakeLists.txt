cmake_minimum_required(VERSION 2.8.11)
project(${WatchDogApiDllName})

set(WatchDogApiDllNameSourcesDir src)
set(WatchDogApiDllNameHeadersDir include)

set(CommonLibHeadersDir ../common/include)

aux_source_directory(${WatchDogApiDllNameSourcesDir} WatchDogApiDllNameSourcesList)
file(GLOB_RECURSE WatchDogApiDllNameHeadersList "include/*.h")

list(REMOVE_ITEM WatchDogApiDllNameSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" WatchDogApiDllNameSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
 	${WatchDogApiDllName} SHARED
 	${WatchDogApiDllNameSourcesList} 
 	${WatchDogApiDllNameHeadersList}
)

add_definitions(
	${QT_DEFINITIONS}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/permissive-
		/std:c++latest
	)
	
	set_target_properties(
		${WatchDogApiDllName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
		RUNTIME_OUTPUT_DIRECTORY "../quickiewebbot/"
	)

endif()

include_directories(
	${WatchDogApiDllNameHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WatchDogApiDllName}
	${Boost_LIBRARIES}
	Qt5::Network
)