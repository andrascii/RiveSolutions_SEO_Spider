cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)
project(${CrawlerEngineLibName})

set(CrawlerEngineSourcesDir src)
set(CrawlerEngineHeadersDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo)
set(MyHtmlLibHeadersDir ../myhtml/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(VMProtectSdkDir ../deps/vmprotect/include)

aux_source_directory(${CrawlerEngineSourcesDir} CrawlerEngineSourcesList)
file(GLOB_RECURSE CrawlerEngineHeadersList "include/*.h")
file(GLOB_RECURSE CrawlerEngineResourcesList "rcc/*.qrc")

qt5_wrap_cpp(CrawlerEngineHeadersMocList ${CrawlerEngineFormsHeadersList})

list(REMOVE_ITEM CrawlerEngineSourcesList src/stdafx.cpp)

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
	)
	
	set_target_properties(
		${CrawlerEngineLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		LINK_FLAGS /DEBUG
	)

endif()

include_directories(
	${CrawlerEngineHeadersDir}
	${GumboLibHeadersDir}
	${MyHtmlLibHeadersDir}
	${CommonLibHeadersDir}
	${VMProtectSdkDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
	set(OSSpecificDeps ws2_32.lib ntdll.lib)
endif()

target_link_libraries(
	${CrawlerEngineLibName}
	${GumboLibName}
	${CommonLibName}
	${OSSpecificDeps}
	"${MyHtmlLibName}_static" # change this solution to that which is more reliable
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Network
	Qt5::Xml
	Qt5::XmlPatterns
)