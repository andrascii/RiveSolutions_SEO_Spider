cmake_minimum_required(VERSION 2.8.11)
project(${WebCrawlerLibName})

set(WebCrawlerSourcesDir src)
set(WebCrawlerHeadersDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${WebCrawlerSourcesDir} WebCrawlerSourcesList)
file(GLOB_RECURSE WebCrawlerHeadersList "include/*.h")
file(GLOB_RECURSE WebCrawlerFormsList "ui/*.ui")
file(GLOB_RECURSE WebCrawlerResourcesList "rcc/*.qrc")

qt5_wrap_ui(WebCrawlerFormsHeadersList ${WebCrawlerFormsList})
qt5_wrap_cpp(WebCrawlerHeadersMocList ${WebCrawlerFormsHeadersList})
qt5_add_resources(WebCrawlerResourcesListAdded ${WebCrawlerResourcesList})

list(REMOVE_ITEM WebCrawlerSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" WebCrawlerSourcesList)

source_group(
	"Generated Files" FILES 
	${WebCrawlerHeadersMocList} 
	${WebCrawlerResourcesListAdded} 
	${WebCrawlerResourcesList} 
	${WebCrawlerFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${WebCrawlerFormsList})
source_group(Qrc FILES ${WebCrawlerResourcesList})

add_library(
 	${WebCrawlerLibName} 
 	${WebCrawlerSourcesList} 
 	${WebCrawlerHeadersList} 
 	${WebCrawlerFormsList} 
 	${WebCrawlerResourcesList}
 	${WebCrawlerFormsHeadersList}
 	${WebCrawlerHeadersMocList}
 	${WebCrawlerResourcesListAdded}
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
		${WebCrawlerLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)

endif()

include_directories(
	${WebCrawlerHeadersDir}
	${GumboLibHeadersDir}
	${CommonLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WebCrawlerLibName}
	${GumboLibName}
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Network
)