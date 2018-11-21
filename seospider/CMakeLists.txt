cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)
project(${AppName})

set(AppSourcesDir src)
set(AppHeadersDir include)

find_package(QtXlsxWriter REQUIRED)

# deps headers
set(CrawlerEngineLibHeadersDir ../crawlerengine/include)
set(CommonLibHeadersDir ../common/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(QtXlsxWriterHeadersDir ../qtxlsxwriter/src/xlsx)
set(VersionHeadersDir ../version/include)
set(VMProtectSdkDir ../deps/vmprotect/include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
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

# this must be before add_executable!
link_directories(${VMProtectSdkLibDir})
message("VMProtectSDK path: ${VMProtectSdkLibDir}")

if(WIN32)
	source_group(WinResource FILES ${AppWin32Resoure})
	
	configure_msvc_runtime()
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
	configure_msvc_runtime()
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

add_custom_command(
	TARGET ${AppName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProctectSdkDll}
	"${CMAKE_CURRENT_BINARY_DIR}/Release"
)

add_custom_command(
	TARGET ${AppName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProctectSdkDll}
	"${CMAKE_CURRENT_BINARY_DIR}/Debug"
)

add_custom_command(
	TARGET ${AppName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProtectDebugLicenseFile}
	"${CMAKE_CURRENT_BINARY_DIR}/Debug"
)

add_definitions(
	${QT_DEFINITIONS}
	-DQUAZIP_STATIC
	-D_WINSOCK_DEPRECATED_NO_WARNINGS
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
	)
	
	set_target_properties(
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
	)
endif()

include_directories(
	${AppHeadersDir}
	${CrawlerEngineLibHeadersDir}
	${CurlHeadersDir}
	${CommonLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${VersionHeadersDir}
	${VMProtectSdkDir}
	${QtXlsxWriterHeadersDir}
	${QtXlsxWriter_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${OPENSSL_INCLUDE_DIR}
)

if(WIN32)
	set(OSSpecificDeps ntdll.lib)
endif()

message("CURL library dependencies: ${CurlLibraries}")
message("OpenSSL library dependencies: ${OPENSSL_LIBRARIES}")
message("OpenSSL include directory: ${OPENSSL_INCLUDE_DIR}")

target_link_libraries(
	${AppName}
	${CurlLibraries}
	${CrawlerEngineLibName}
	${CommonLibName}
	${VersionProjectName}
	${QtXlsxWriter_LIBS}
	${OSSpecificDeps}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${OPENSSL_LIBRARIES}
	D:/deps/openssl/openssl_$ENV{SS_PLATFORM}/lib/libcrypto.lib
	D:/deps/openssl/openssl_$ENV{SS_PLATFORM}/lib/libssl.lib
	Qt5::Widgets
	Qt5::Network
	Qt5::Svg
	Qt5::WebEngineWidgets
	Qt5::PrintSupport
)