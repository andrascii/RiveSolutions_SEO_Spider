cmake_minimum_required(VERSION 2.8.11)
project(${AppName})

set(AppSourcesDir src)
set(AppHeadersDir include)

# deps headers
set(WebCrawlerLibHeadersDir ../webcrawler/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppCppList "src/*.cpp")
file(GLOB_RECURSE AppHeadersList "include/*.h")
file(GLOB_RECURSE AppFormsList "ui/*.ui")
file(GLOB_RECURSE AppResourcesList "rcc/*.qrc")

qt5_wrap_ui(AppFormsHeadersList ${AppFormsList})
qt5_wrap_cpp(AppHeadersMocList ${AppFormsHeadersList})
qt5_add_resources(AppResourcesListAdded ${AppResourcesList})

list(REMOVE_ITEM AppSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" AppSourcesList)

source_group(
	"Generated Files" FILES 
	${AppHeadersMocList} 
	${AppResourcesListAdded} 
	${AppResourcesList} 
	${AppFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${AppFormsList})
source_group(Qrc FILES ${AppResourcesList})

add_executable(
	${AppName} 
	${AppSourcesList} 
	${AppHeadersList} 
	${AppFormsList} 
	${AppResourcesList}
	${AppFormsHeadersList}
	${AppHeadersMocList}
	${AppResourcesListAdded}
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
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${AppHeadersDir}
	${WebCrawlerLibHeadersDir}
	${CommonLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(
	${AppName}
	${WebCrawlerLibName}
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${OPENSSL_LIBRARIES}
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
)