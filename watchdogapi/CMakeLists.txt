cmake_minimum_required(VERSION 2.8.11)
project(${WatchDogApi})

set(WatchDogApiSourcesDir src)
set(WatchDogApiHeadersDir include)

# deps headers
set(CommonLibHeadersDir ../common/include)

aux_source_directory(${WatchDogApiSourcesDir} WatchDogApiSourcesList)
file(GLOB_RECURSE WatchDogApiHeadersList "include/*.h")

list(REMOVE_ITEM WatchDogApiSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" WatchDogApiSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
 	${WatchDogApi} SHARED
 	${WatchDogApiSourcesList} 
 	${WatchDogApiHeadersList}
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
		${WatchDogApi}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
	)

endif()

include_directories(
	${WatchDogApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WatchDogApi}
	${Boost_LIBRARIES}
	Qt5::Network
)