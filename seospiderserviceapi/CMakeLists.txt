cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)
project(${SeoSpiderServiceApiDllName})

set(SeoSpiderServiceApiDllNameSourcesDir src)
set(SeoSpiderServiceApiDllNameHeadersDir include)
set(CommonLibHeadersDir ../common/include)

aux_source_directory(${SeoSpiderServiceApiDllNameSourcesDir} SeoSpiderServiceApiDllNameSourcesList)
file(GLOB_RECURSE SeoSpiderServiceApiDllNameHeadersList "include/*.h")

list(REMOVE_ITEM SeoSpiderServiceApiDllNameSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" SeoSpiderServiceApiDllNameSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()
add_library(
 	${SeoSpiderServiceApiDllName} SHARED
 	${SeoSpiderServiceApiDllNameSourcesList} 
 	${SeoSpiderServiceApiDllNameHeadersList}
)

add_definitions(
	${QT_DEFINITIONS}
	-DQUAZIP_STATIC
)

if(MSVC)

	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/permissive-
		/Zi
		/W4
		/WX
	)
	
	set_target_properties(
		${SeoSpiderServiceApiDllName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		RUNTIME_OUTPUT_DIRECTORY "../seospider/"
		LINK_FLAGS /DEBUG
	)

endif()

include_directories(
	${SeoSpiderServiceApiDllNameHeadersDir}
	${CommonLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${SeoSpiderServiceApiDllName}
	${CommonLibName}
	${Boost_LIBRARIES}
	Qt5::Network
)