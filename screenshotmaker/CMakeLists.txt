cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
set (CMAKE_CXX_STANDARD 17)

include(cotire)

project(${ScreenshotMaker})

set(AppSourcesDir src)
set(AppHeadersDir include)

# deps headers
set(CrawlerEngineLibHeadersDir ../crawlerengine/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(QtXlsxWriterHeadersDir ../qtxlsxwriter/src/xlsx)
set(VersionHeadersDir ../version/include)
set(VMProtectSdkDir ../deps/vmprotect/include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppCppList "src/*.cpp")
file(GLOB_RECURSE AppHeadersList "include/*.h")
file(GLOB_RECURSE AppResourcesList "rcc/*.qrc")
file(GLOB_RECURSE AppWin32Resoure "rcc/*.rc")

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
source_group(Qrc FILES ${AppResourcesList})

if(WIN32)
	source_group(WinResource FILES ${AppWin32Resoure})
	
	configure_msvc_runtime()
	add_executable(
		${ScreenshotMaker} 
		${AppSourcesList}
		${AppHeadersList}
		${AppResourcesList}
		${AppFormsHeadersList}
		${AppHeadersMocList}
		${AppResourcesListAdded}
		${AppWin32Resoure}
	)
else()
	configure_msvc_runtime()
	add_executable(
		${ScreenshotMaker} 
		${AppSourcesList}
		${AppHeadersList}
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
		/std:c++17
		/Zi
		/W4
		/WX
	)
	
	set_target_properties(
		${ScreenshotMaker}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${ScreenshotMaker} 
		PROPERTIES WIN32_EXECUTABLE YES
		RUNTIME_OUTPUT_DIRECTORY "../seospider/"
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
	)
else()
    set_target_properties(
        ${ScreenshotMaker}
        PROPERTIES WIN32_EXECUTABLE YES
        RUNTIME_OUTPUT_DIRECTORY "../seospider/"
    )
endif()

include_directories(
	${AppHeadersDir}
	${VersionHeadersDir}
	${CommonLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
	set(OSSpecificDeps ntdll.lib)
endif()

target_link_libraries(
	${ScreenshotMaker}
	PUBLIC
	${CommonLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${OPENSSL_LIBRARIES}
	Qt5::WebEngineWidgets
)

if(NOT WIN32)
	# to speedup compilation
	# but qtsvg can't be compiled with cotire
	# this is because we call cotire() here
	set_target_properties(${ScreenshotMaker} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/stdafx.h")
	cotire(${ScreenshotMaker})
endif()
