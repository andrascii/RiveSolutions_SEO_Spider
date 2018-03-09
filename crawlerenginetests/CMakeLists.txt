cmake_minimum_required(VERSION 2.8.11)
project(${CrawlerEngineLibTestsName})

set(CrawlerEngineTestsSourceDir src)
set(CrawlerEngineTestsHeaderDir include)

# deps headers
set(CrawlerEngineLibHeadersDir ../crawlerengine/include)
set(CommonLibHeadersDir ../common/include)
set(WatchDogApiHeadersDir ../watchdogapi/include)
set(GoogleTestHeadersDir ../googletest/googletest/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${CrawlerEngineTestsSourceDir} CrawlerEngineTestsSourcesList)
file(GLOB_RECURSE CrawlerEngineTestsHeaderList "include/*.h")

qt5_wrap_cpp(CrawlerEngineTestsHeadersMocList ${CrawlerEngineTestsFormsHeadersList})

list(REMOVE_ITEM CrawlerEngineTestsSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CrawlerEngineTestsSourcesList)

source_group(
	"Generated Files" FILES 
	${CrawlerEngineTestsHeadersMocList} 
	${CrawlerEngineTestsResourcesListAdded} 
	${CrawlerEngineTestsFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_executable(
	${CrawlerEngineLibTestsName} 
	${CrawlerEngineTestsSourcesList} 
	${CrawlerEngineTestsHeaderList} 
	${CrawlerEngineTestsFormsHeadersList}
	${CrawlerEngineTestsHeadersMocList}
	${CrawlerEngineTestsResourcesListAdded}
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
		${CrawlerEngineLibTestsName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${CrawlerEngineLibTestsName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup"
	)

	set_target_properties(
		${CrawlerEngineLibTestsName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		RUNTIME_OUTPUT_DIRECTORY "../seospider/"
	)
endif()

include_directories(
	${CrawlerEngineTestsHeaderDir}
	${CrawlerEngineLibHeadersDir}
	${CommonLibHeadersDir}
	${WatchDogApiHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${CrawlerEngineLibTestsName}
	${CrawlerEngineLibName}
	gtest
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	Qt5::Widgets
	Qt5::Network
	Qt5::Xml
	Qt5::XmlPatterns
)