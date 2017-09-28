cmake_minimum_required(VERSION 2.8.11)
project(${SeoSpiderServiceApiDllName})

set(SeoSpiderServiceApiDllNameSourcesDir src)
set(SeoSpiderServiceApiDllNameHeadersDir include)
set(CommonLibHeadersDir ../common/include)

aux_source_directory(${SeoSpiderServiceApiDllNameSourcesDir} SeoSpiderServiceApiDllNameSourcesList)
file(GLOB_RECURSE SeoSpiderServiceApiDllNameHeadersList "include/*.h")

list(REMOVE_ITEM SeoSpiderServiceApiDllNameSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" SeoSpiderServiceApiDllNameSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
 	${SeoSpiderServiceApiDllName} SHARED
 	${SeoSpiderServiceApiDllNameSourcesList} 
 	${SeoSpiderServiceApiDllNameHeadersList}
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
		/D EXPORT
	)
	
	set_target_properties(
		${SeoSpiderServiceApiDllName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
		RUNTIME_OUTPUT_DIRECTORY "../quickiewebbot/"
	)

endif()

include_directories(
	${SeoSpiderServiceApiDllNameHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	Qt5::Network
)