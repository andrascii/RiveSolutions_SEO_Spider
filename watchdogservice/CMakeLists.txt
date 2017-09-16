cmake_minimum_required(VERSION 2.8.11)
project(${WatchDogServiceName})

set(WatchDogServiceSourcesDir src)
set(WatchDogServiceHeadersDir include)

set(CommonLibHeadersDir ../common/include)

aux_source_directory(${WatchDogServiceSourcesDir} WatchDogServiceSourcesList)
file(GLOB_RECURSE WatchDogServiceHeadersList "include/*.h")
file(GLOB_RECURSE WatchDogServiceFormsList "ui/*.ui")
file(GLOB_RECURSE WatchDogServiceResourcesList "rcc/*.qrc")

qt5_wrap_ui(WatchDogServiceFormsHeadersList ${WatchDogServiceFormsList})
qt5_wrap_cpp(WatchDogServiceHeadersMocList ${WatchDogServiceFormsHeadersList})
qt5_add_resources(WatchDogServiceResourcesListAdded ${WatchDogServiceResourcesList})

list(REMOVE_ITEM WatchDogServiceSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" WatchDogServiceSourcesList)

source_group(
	"Generated Files" FILES 
	${WatchDogServiceHeadersMocList} 
	${WatchDogServiceResourcesListAdded} 
	${WatchDogServiceResourcesList} 
	${WatchDogServiceFormsHeadersList}
)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)
source_group(Ui FILES ${WatchDogServiceFormsList})
source_group(Qrc FILES ${WatchDogServiceResourcesList})

add_executable(
 	${WatchDogServiceName}
 	${WatchDogServiceSourcesList} 
 	${WatchDogServiceHeadersList}
	${WatchDogServiceHeadersMocList} 
	${WatchDogServiceResourcesListAdded} 
	${WatchDogServiceResourcesList} 
	${WatchDogServiceFormsHeadersList}
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
		${WatchDogServiceName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
		RUNTIME_OUTPUT_DIRECTORY "../quickiewebbot/"
	)
	
endif()

include_directories(
	${WatchDogServiceHeadersDir}
	${CommonLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${WatchDogServiceName}
	${CommonLibName}
	${Boost_LIBRARIES}
	Qt5::Widgets
)