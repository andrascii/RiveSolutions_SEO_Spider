cmake_minimum_required(VERSION 3.2)
project(${AppName})

set(AppSourcesDir src)
set(AppHeadersDir include)

find_package(QtXlsxWriter REQUIRED)

# deps headers
set(CrawlerEngineLibHeadersDir ../crawlerengine/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(QtXlsxWriterHeadersDir ../qtxlsxwriter/src/xlsx)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppCppList "src/*.cpp")
file(GLOB_RECURSE AppHeadersList "include/*.h")
file(GLOB_RECURSE AppFormsList "ui/*.ui")
file(GLOB_RECURSE AppResourcesList "rcc/*.qrc")
file(GLOB_RECURSE AppWin32Resoure "rcc/*.rc")

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

if(WIN32)
	source_group(WinResource FILES ${AppWin32Resoure})
	
	add_executable(
		${AppName} 
		${AppSourcesList}
		${AppHeadersList}
		${AppFormsList} 
		${AppResourcesList}
		${AppFormsHeadersList}
		${AppHeadersMocList}
		${AppResourcesListAdded}
		${AppWin32Resoure}
	)
else()
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
endif()

add_definitions(
	${QT_DEFINITIONS}
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
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${AppHeadersDir}
	${CrawlerEngineLibHeadersDir}
	${CommonLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${QtXlsxWriterHeadersDir}
	${QtXlsxWriter_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(
	${AppName}
	${CrawlerEngineLibName}
	${CommonLibName}
	${SeoSpiderServiceApiDllName}
	${QtXlsxWriter_LIBS}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${OPENSSL_LIBRARIES}
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
)