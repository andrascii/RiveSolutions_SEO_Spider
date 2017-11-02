cmake_minimum_required(VERSION 2.8.11)
project(${WebCrawlerLibTestsName})

set(WebCrawlerTestsSourceDir src)
set(WebCrawlerTestsHeaderDir include)

# deps headers
set(WebCrawlerLibHeadersDir ../webcrawler/include)
set(CommonLibHeadersDir ../common/include)
set(WatchDogApiHeadersDir ../watchdogapi/include)
set(GoogleTestHeadersDir ../googletest/googletest/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${WebCrawlerTestsSourceDir} WebCrawlerTestsSourcesList)
file(GLOB_RECURSE WebCrawlerTestsHeaderList "include/*.h")
file(GLOB_RECURSE WebCrawlerTestsFormsList "ui/*.ui")
file(GLOB_RECURSE WebCrawlerTestsResourcesList "rcc/*.qrc")

qt5_wrap_ui(WebCrawlerTestsFormsHeadersList ${WebCrawlerTestsFormsList})
qt5_wrap_cpp(WebCrawlerTestsHeadersMocList ${WebCrawlerTestsFormsHeadersList})
qt5_add_resources(WebCrawlerTestsResourcesListAdded ${WebCrawlerTestsResourcesList})

list(REMOVE_ITEM WebCrawlerTestsSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" WebCrawlerTestsSourcesList)

source_group(
	"Generated Files" FILES 
	${WebCrawlerTestsHeadersMocList} 
	${WebCrawlerTestsResourcesListAdded} 
	${WebCrawlerTestsResourcesList} 
	${WebCrawlerTestsFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${WebCrawlerTestsFormsList})
source_group(Qrc FILES ${WebCrawlerTestsResourcesList})

add_executable(
	${WebCrawlerLibTestsName} 
	${WebCrawlerTestsSourcesList} 
	${WebCrawlerTestsHeaderList} 
	${WebCrawlerTestsFormsList} 
	${WebCrawlerTestsResourcesList}
	${WebCrawlerTestsFormsHeadersList}
	${WebCrawlerTestsHeadersMocList}
	${WebCrawlerTestsResourcesListAdded}
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
		${WebCrawlerLibTestsName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${WebCrawlerLibTestsName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)

	set_target_properties(
		${WebCrawlerLibTestsName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		RUNTIME_OUTPUT_DIRECTORY "../seospider/"
	)
endif()

include_directories(
	${WebCrawlerTestsHeaderDir}
	${WebCrawlerLibHeadersDir}
	${CommonLibHeadersDir}
	${WatchDogApiHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WebCrawlerLibTestsName}
	${WatchDogApiDllName}
	${WebCrawlerLibName}
	gtest
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
)