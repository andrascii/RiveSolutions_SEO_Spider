cmake_minimum_required(VERSION 2.8.11)
project(${CrawlerEngineLibName})

set(CrawlerEngineSourcesDir src)
set(CrawlerEngineHeadersDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${CrawlerEngineSourcesDir} CrawlerEngineSourcesList)
file(GLOB_RECURSE CrawlerEngineHeadersList "include/*.h")
file(GLOB_RECURSE CrawlerEngineFormsList "ui/*.ui")
file(GLOB_RECURSE CrawlerEngineResourcesList "rcc/*.qrc")

qt5_wrap_ui(CrawlerEngineFormsHeadersList ${CrawlerEngineFormsList})
qt5_wrap_cpp(CrawlerEngineHeadersMocList ${CrawlerEngineFormsHeadersList})
qt5_add_resources(CrawlerEngineResourcesListAdded ${CrawlerEngineResourcesList})

list(REMOVE_ITEM CrawlerEngineSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CrawlerEngineSourcesList)

source_group(
	"Generated Files" FILES 
	${CrawlerEngineHeadersMocList} 
	${CrawlerEngineResourcesListAdded} 
	${CrawlerEngineResourcesList} 
	${CrawlerEngineFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${CrawlerEngineFormsList})
source_group(Qrc FILES ${CrawlerEngineResourcesList})

add_library(
 	${CrawlerEngineLibName} 
 	${CrawlerEngineSourcesList} 
 	${CrawlerEngineHeadersList} 
 	${CrawlerEngineFormsList} 
 	${CrawlerEngineResourcesList}
 	${CrawlerEngineFormsHeadersList}
 	${CrawlerEngineHeadersMocList}
 	${CrawlerEngineResourcesListAdded}
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
		${CrawlerEngineLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)

endif()

include_directories(
	${CrawlerEngineHeadersDir}
	${GumboLibHeadersDir}
	${CommonLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${CrawlerEngineLibName}
	${GumboLibName}
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Network
)