cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

set(QUazipSourcesDir src)
set(QUazipHeadersDir include)

# deps headers
aux_source_directory(${QUazipSourcesDir} QUazipSourcesList)
file(GLOB_RECURSE QUazipHeadersList "include/*.h")

#list(REMOVE_ITEM QUazipSourcesList src/stdafx.cpp)
#add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" QUazipSourcesList)
#source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

#SET(ZLIB_NAMES z zlib zdll zlib1 zlibd zlibd1 zlibwapi)

configure_msvc_runtime()
add_library(
	${QUazipLibName} 
	${QUazipSourcesList} 
	${QUazipHeadersList} 
)

add_definitions(
	${QT_DEFINITIONS}
	-DQUAZIP_STATIC
	-DQUAZIP_BUILD
	-D_CRT_SECURE_NO_WARNINGS
)

remove_definitions(
	-DQT_NO_CAST_FROM_ASCII
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/permissive-
		/std:c++latest
		/Zi
		/W3
		/WX
	)
	
	set_target_properties(
		${CommonLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${QUazipLibName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${QUazipHeadersDir}
)

target_link_libraries(
	${QUazipLibName}
	PUBLIC
	zlibstatic
	Qt5::Core
)