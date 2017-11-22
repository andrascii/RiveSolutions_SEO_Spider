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

###########################################################################
#
# This project must be configured without /permissive- flag
# because we use Boost.Log lib and that lib includes typeof.hpp file
# that also includes typeof_impl.hpp which uses old Visual Studio bugs.
# These bugs were fixed with /permissive- flag and therefore with it
# project will not compile.
#
###########################################################################

	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/D EXPORT
		/Zi
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
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	Qt5::Network
)