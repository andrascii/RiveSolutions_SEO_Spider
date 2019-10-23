cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
set(CMAKE_CXX_STANDARD 17)

include(cotire)

set(CommonSourcesDir src)
set(CommonHeadersDir include)

# deps headers
aux_source_directory(${CommonSourcesDir} CommonSourcesList)
file(GLOB_RECURSE CommonHeadersList "include/*.h")
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(QxtSmtpLibHeadersDir ../qxtsmtp/include)
set(QUazipLibHeadersDir ../quazip/include)

if (APPLE OR UNIX)
	list(REMOVE_ITEM CommonSourcesList
		src/stdafx.cpp
		src/ipc_socket.cpp
		src/ipc_server.cpp
	)
else()
	list(REMOVE_ITEM CommonSourcesList
		src/stdafx.cpp
		src/tcp_socket.cpp
		src/tcp_server.cpp
	)
endif()

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" CommonSourcesList)
source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()
add_library(
	${CommonLibName} 
	${CommonSourcesList} 
	${CommonHeadersList} 
)

add_definitions(
	${QT_DEFINITIONS}
	-DQXT_STATIC
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
		${CommonLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${CommonLibName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${CommonHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${QxtSmtpLibHeadersDir}
	${QUazipLibHeadersDir}
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(
	${CommonLibName}
	PUBLIC
	${QxtSmtpLibName}
	${QUazipLibName}
	Qt5::Svg
	Qt5::Widgets
	Qt5::Network
	Qt5::WebEngineWidgets
)

if(NOT WIN32)
	# to speedup compilation
	# but qtsvg can't be compiled with cotire
	# this is because we call cotire() here
	set_target_properties(${CommonLibName} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/stdafx.h")
	cotire(${CommonLibName})
endif()