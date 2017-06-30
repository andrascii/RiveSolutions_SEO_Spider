cmake_minimum_required(VERSION 2.8.11)

project(${CommonLibName})

set(CommonLibSourcesDir src)
set(CommonLibHeadersDir include)

# prepare headers directories and sources list
set(AllHeadersDir
	${CommonLibHeadersDir} 
)

aux_source_directory(${CommonLibSourcesDir} CommonLibSourcesList)
file(GLOB_RECURSE CommonLibHeadersList "include/*.h")

set(AllSources ${CommonLibSourcesList})
set(AllHeaders ${CommonLibHeadersList})

list(REMOVE_ITEM AllSources src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" AllSources)

add_library(
	${CommonLibName} STATIC
	${AllSources} 
	${AllHeaders}
)

if(MSVC)
	add_definitions(
		${QT_DEFINITIONS}
		/MP
		/permissive-
	)
	
	set_target_properties(
		${CommonLibName} 
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
endif()

include_directories(
	${AllHeadersDir}
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(
	${CommonLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Widgets
)