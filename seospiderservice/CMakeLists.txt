cmake_minimum_required(VERSION 2.8.11)
project(${SeoSpiderServiceAppName})

set(SeoSpiderServiceSourcesDir src)
set(SeoSpiderServiceHeadersDir include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include/)
set(CommonLibHeadersDir ../common/include)
set(VersionHeadersDir ../version/include)

aux_source_directory(${SeoSpiderServiceSourcesDir} SeoSpiderServiceSourcesList)
file(GLOB_RECURSE SeoSpiderServiceHeadersList "include/*.h")
file(GLOB_RECURSE SeoSpiderServiceFormsList "ui/*.ui")
file(GLOB_RECURSE SeoSpiderServiceResourcesList "rcc/*.qrc")

qt5_wrap_ui(SeoSpiderServiceFormsHeadersList ${SeoSpiderServiceFormsList})
qt5_wrap_cpp(SeoSpiderServiceHeadersMocList ${SeoSpiderServiceFormsHeadersList})
qt5_add_resources(SeoSpiderServiceResourcesListAdded ${SeoSpiderServiceResourcesList})

list(REMOVE_ITEM SeoSpiderServiceSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" SeoSpiderServiceSourcesList)

source_group(
	"Generated Files" FILES 
	${SeoSpiderServiceHeadersMocList} 
	${SeoSpiderServiceResourcesListAdded} 
	${SeoSpiderServiceResourcesList} 
	${SeoSpiderServiceFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${SeoSpiderServiceFormsList})
source_group(Qrc FILES ${SeoSpiderServiceResourcesList})

add_executable(
 	${SeoSpiderServiceAppName}
 	${SeoSpiderServiceSourcesList} 
 	${SeoSpiderServiceHeadersList}
	${SeoSpiderServiceHeadersMocList} 
	${SeoSpiderServiceResourcesListAdded} 
	${SeoSpiderServiceResourcesList} 
	${SeoSpiderServiceFormsHeadersList}
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
		/Zi
		/W4
		/WX
	)
	
	set_target_properties(
		${SeoSpiderServiceAppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		RUNTIME_OUTPUT_DIRECTORY "../seospider/"
		LINK_FLAGS /DEBUG
	)
	
endif()

include_directories(
	${SeoSpiderServiceHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${CommonLibHeadersDir}
	${VersionHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

if (MSVC)
	target_link_libraries(
		${SeoSpiderServiceAppName}
		${CommonLibName}
		#${QxtSmtpLibName}
		${Boost_LIBRARIES}
		Qt5::Widgets
		Qt5::Network
		Mincore.lib
	)
endif()


if(APPLE)
    target_link_libraries(
		${SeoSpiderServiceAppName}
		${CommonLibName}
		#${QxtSmtpLibName}
		${Boost_LIBRARIES}
		Qt5::Widgets
		Qt5::Network
	)
endif()