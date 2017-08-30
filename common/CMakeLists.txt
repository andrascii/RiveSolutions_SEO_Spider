cmake_minimum_required(VERSION 2.8.11)

set(CommonSourcesDir src)
set(CommonHeadersDir include)

# deps headers
aux_source_directory(${CommonSourcesDir} CommonSourcesList)
file(GLOB_RECURSE CommonHeadersList "include/*.h")

list(REMOVE_ITEM CommonSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CommonSourcesList)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
	${CommonLibName} 
	${CommonSourcesList} 
	${CommonHeadersList} 
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
		${CommonLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
	)
	
	set_target_properties(
		${CommonLibName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${CommonHeadersDir}
)

target_link_libraries(
	${CommonLibName}
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
)