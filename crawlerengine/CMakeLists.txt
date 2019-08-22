cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
set(CMAKE_CXX_STANDARD 17)

include(cotire)

project(${CrawlerEngineLibName})

set(CrawlerEngineSourcesDir src)
set(CrawlerEngineHeadersDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo)
set(MyHtmlLibHeadersDir ../myhtml/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${CrawlerEngineSourcesDir} CrawlerEngineSourcesList)
file(GLOB_RECURSE CrawlerEngineHeadersList "include/*.h")
file(GLOB_RECURSE CrawlerEngineResourcesList "rcc/*.qrc")

qt5_wrap_cpp(CrawlerEngineHeadersMocList ${CrawlerEngineFormsHeadersList})

list(REMOVE_ITEM CrawlerEngineSourcesList src/stdafx.cpp src/screenshot_maker.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CrawlerEngineSourcesList)

source_group(
	"Generated Files" FILES 
	${CrawlerEngineHeadersMocList} 
	${CrawlerEngineResourcesListAdded} 
	${CrawlerEngineFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()

add_library(
 	${CrawlerEngineLibName} 
 	${CrawlerEngineSourcesList} 
 	${CrawlerEngineHeadersList} 
 	${CrawlerEngineFormsHeadersList}
 	${CrawlerEngineHeadersMocList}
 	${CrawlerEngineResourcesListAdded}
)

add_definitions(
	${QT_DEFINITIONS}
	-DQUAZIP_STATIC
	-DCURL_STATICLIB
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/permissive-
		/std:c++latest
		/Zi
		/W4
		/WX
		-D_WINSOCK_DEPRECATED_NO_WARNINGS
	)
	
	set_target_properties(
		${CrawlerEngineLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		LINK_FLAGS /DEBUG
	)
endif()

if(APPLE OR UNIX)
	include_directories(
		${CrawlerEngineHeadersDir}
		${GumboLibHeadersDir}
		${MyHtmlLibHeadersDir}
		${CommonLibHeadersDir}
		${VMProtectSdkHeadersDir}
		${SeoSpiderServiceApiHeadersDir}
		${Boost_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
		${CURL_INCLUDE_DIRS}
	)
else()
	message("CURL include headers directory: ${CurlHeadersDir}")

	include_directories(
		${CrawlerEngineHeadersDir}
		${CurlHeadersDir}
		${GumboLibHeadersDir}
		${MyHtmlLibHeadersDir}
		${CommonLibHeadersDir}
		${VMProtectSdkHeadersDir}
		${SeoSpiderServiceApiHeadersDir}
		${Boost_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

if(APPLE OR UNIX)
	target_link_libraries(
		${CrawlerEngineLibName}
		PUBLIC
		${GumboLibName}
		${CommonLibName}
		${OSSpecificDeps}
		${CURL_LIBRARIES}
		"${MyHtmlLibName}_static" # change this solution to that which is more reliable
		${Boost_LIBRARIES}
		${Boost_SYSTEM_LIBRARY}
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_ASIO_LIBRARY}
		${Boost_REGEX_LIBRARY}
		Qt5::Network
		Qt5::Xml
		Qt5::XmlPatterns
		Qt5::WebEngineWidgets
	)
else()
	add_dependencies(${CrawlerEngineLibName} ${CurlDependency})
	
	target_link_libraries(
		${CrawlerEngineLibName}
		PUBLIC
		${GumboLibName}
		${CommonLibName}
		ws2_32.lib
		ntdll.lib
		${CurlLibraries}
		"${MyHtmlLibName}_static" # change this solution to that which is more reliable
		${Boost_LIBRARIES}
		${Boost_SYSTEM_LIBRARY}
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_ASIO_LIBRARY}
		${Boost_REGEX_LIBRARY}
		Qt5::Network
		Qt5::Xml
		Qt5::XmlPatterns
		Qt5::WebEngineWidgets
	)
endif()

set_target_properties(${CrawlerEngineLibName} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/stdafx.h")
cotire(${CrawlerEngineLibName})