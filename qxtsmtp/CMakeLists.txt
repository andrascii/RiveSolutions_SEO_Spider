cmake_minimum_required(VERSION 2.8.11)

set(QxtSmtpSourcesDir src)
set(QxtSmtpHeadersDir include)

# deps headers
aux_source_directory(${QxtSmtpSourcesDir} QxtSmtpSourcesList)
file(GLOB_RECURSE QxtSmptHeadersList "include/*.h")

#list(REMOVE_ITEM QxtSmtpSourcesList src/stdafx.cpp)
#add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" QxtSmtpSourcesList)
#source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
	${QxtSmtpLibName} 
	${QxtSmtpSourcesList} 
	${QxtSmptHeadersList} 
)

add_definitions(
	${QT_DEFINITIONS}
	-DQXT_STATIC
	-DBUILD_QXT_CORE
	-DBUILD_QXT_NETWORK
	-DQT_DISABLE_DEPRECATED_BEFORE=0
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
	)
	
	set_target_properties(
		${CommonLibName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${QxtSmtpLibName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${QxtSmtpHeadersDir}
)

target_link_libraries(
	${QxtSmtpLibName}
	Qt5::Network
)