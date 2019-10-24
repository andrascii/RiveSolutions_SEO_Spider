cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
set (CMAKE_CXX_STANDARD 17)

project(${CrawlerEngineLibTestsName})

set(CrawlerEngineTestsSourceDir src)
set(CrawlerEngineTestsHeaderDir include)

# deps headers
set(GumboLibHeadersDir ../gumbo)
set(CrawlerEngineLibHeadersDir ../crawlerengine/include)
set(CommonLibHeadersDir ../common/include)
set(GoogleTestHeadersDir ../googletest/googletest/include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(MyHtmlLibHeadersDir ../myhtml/include)

aux_source_directory(${CrawlerEngineTestsSourceDir} CrawlerEngineTestsSourcesList)
file(GLOB_RECURSE CrawlerEngineTestsHeaderList "include/*.h")

qt5_wrap_cpp(CrawlerEngineTestsHeadersMocList ${CrawlerEngineTestsFormsHeadersList})

list(REMOVE_ITEM CrawlerEngineTestsSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CrawlerEngineTestsSourcesList)

link_directories(${VMProtectSdkLibDir})

source_group(
	"Generated Files" FILES 
	${CrawlerEngineTestsHeadersMocList} 
	${CrawlerEngineTestsResourcesListAdded} 
	${CrawlerEngineTestsFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()
add_executable(
	${CrawlerEngineLibTestsName} 
	${CrawlerEngineTestsSourcesList} 
	${CrawlerEngineTestsHeaderList} 
	${CrawlerEngineTestsFormsHeadersList}
	${CrawlerEngineTestsHeadersMocList}
	${CrawlerEngineTestsResourcesListAdded}
)

add_custom_command(
	TARGET ${CrawlerEngineLibTestsName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProctectSdkDll}
	"${CMAKE_CURRENT_BINARY_DIR}/Release"
)

add_custom_command(
	TARGET ${CrawlerEngineLibTestsName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProctectSdkDll}
	"${CMAKE_CURRENT_BINARY_DIR}/Debug"
)

add_custom_command(
	TARGET ${CrawlerEngineLibTestsName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy
	${VMProtectDebugLicenseFile}
	"${CMAKE_CURRENT_BINARY_DIR}/Debug"
)

add_definitions(
	${QT_DEFINITIONS}
	-DQUAZIP_STATIC
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

message("${CrawlerEngineLibTestsName}: CURL library dependencies: ${CurlLibraries}")
message("${CrawlerEngineLibTestsName}: CURL library include dir: ${CurlHeadersDir}")

include_directories(
	${CrawlerEngineTestsHeaderDir}
	${CrawlerEngineLibHeadersDir}
	${CurlHeadersDir}
	${CommonLibHeadersDir}
	${GumboLibHeadersDir}
	${MyHtmlLibHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${CrawlerEngineLibTestsName}
	PUBLIC
	${CurlLibraries}
	${CrawlerEngineLibName}
	gtest
	${CommonLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	D:/deps/openssl/openssl_$ENV{SS_PLATFORM}/lib/libcrypto.lib
	D:/deps/openssl/openssl_$ENV{SS_PLATFORM}/lib/libssl.lib
	PUBLIC
	Qt5::Widgets
	Qt5::Network
	Qt5::Xml
	Qt5::XmlPatterns
)