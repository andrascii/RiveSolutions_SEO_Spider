cmake_minimum_required(VERSION 2.8.11)
project(${WebCrawlerLibTestsName})

set(WebCrawlerTestsSourceDir src)
set(WebCrawlerTestsHeaderDir include)

# deps headers
set(WebCrawlerLibHeadersDir ../webcrawler/include)
set(CommonLibHeadersDir ../common/include)
set(WatchDogApiHeadersDir ../watchdogapi/include)
set(GoogleTestHeadersDir ../googletest/googletest/include)

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
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
	)
	
	set_target_properties(
		${WebCrawlerLibTestsName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${WebCrawlerTestsHeaderDir}
	${WebCrawlerLibHeadersDir}
	${CommonLibHeadersDir}
	${WatchDogApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WebCrawlerLibTestsName}
	${WatchDogApiDllName}
	${WebCrawlerLibName}
	gtest
	${CommonLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
)