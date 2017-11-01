cmake_minimum_required(VERSION 2.8.11)
project(${LoggerAppName})

set(LoggerAppSourcesDir src)
set(LoggerAppHeadersDir include)
set(SeoSpiderServiceApiHeadersDir ../seospiderserviceapi/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../${AppName})

aux_source_directory(${LoggerAppSourcesDir} LoggerAppSourcesList)
file(GLOB_RECURSE LoggerAppHeadersList "include/*.h")
file(GLOB_RECURSE LoggerAppFormsList "ui/*.ui")
file(GLOB_RECURSE LoggerAppResourcesList "rcc/*.qrc")

qt5_wrap_ui(LoggerAppFormsHeadersList ${LoggerAppFormsList})
qt5_wrap_cpp(LoggerAppHeadersMocList ${LoggerAppFormsHeadersList})
qt5_add_resources(LoggerAppResourcesListAdded ${LoggerAppResourcesList})

list(REMOVE_ITEM LoggerAppSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" LoggerAppSourcesList)

source_group(
	GeneratedFiles FILES 
	${LoggerAppHeadersMocList} 
	${LoggerAppResourcesListAdded} 
	${LoggerAppResourcesList} 
	${LoggerAppFormsHeadersList}
)

source_group(PrecompiledHeaders FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${LoggerAppFormsList})
source_group(Qrc FILES ${LoggerAppResourcesList})

add_executable(
	${LoggerAppName} 
	${LoggerAppSourcesList} 
	${LoggerAppHeadersList} 
	${LoggerAppFormsList} 
	${LoggerAppResourcesList}
	${LoggerAppFormsHeadersList}
	${LoggerAppHeadersMocList}
	${LoggerAppResourcesListAdded}
)

add_definitions(
	${QT_DEFINITIONS}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/permissive-
	)
	
	set_target_properties(
		${LoggerAppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${LoggerAppName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
	
	set_target_properties(
		${LoogerAppName}
		PROPERTIES 
		RUNTIME_OUTPUT_DIRECTORY "../quickiewebbot"
	)
endif()

include_directories(
	${LoggerAppHeadersDir}
	${SeoSpiderServiceApiHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${LoggerAppName}
	${Boost_LIBRARIES}
	Qt5::Widgets
	Qt5::Network
)