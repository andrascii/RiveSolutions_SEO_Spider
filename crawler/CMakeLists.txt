cmake_minimum_required(VERSION 2.8.11)
project(${CrawlerLibName})

set(CrawlerLibSourcesDir src)
set(CrawlerLibHeadersDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo/include)
set(CommonLibHeadersDir ../common/include)

aux_source_directory(${CrawlerLibSourcesDir} CrawlerLibSourcesList)
file(GLOB_RECURSE CrawlerLibHeadersList "include/*.h")

list(REMOVE_ITEM CrawlerLibSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CrawlerLibSourcesList)

source_group(precompiled_headers FILES include/stdafx.h src/stdafx.cpp)

add_library(
	${CrawlerLibName} 
	${CrawlerLibSourcesList} 
	${CrawlerLibHeadersList} 
)

add_definitions(
	${QT_DEFINITIONS}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/permissive-
	)
	
	set_target_properties(
		${CrawlerLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
	)
	
	set_target_properties(
		${CrawlerLibName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${CrawlerLibHeadersDir}
	${GumboLibHeadersDir}
	${CommonLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${CrawlerLibName}
	${CommonLibName}
	${GumboLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Widgets
	Qt5::Network
)