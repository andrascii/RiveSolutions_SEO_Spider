cmake_minimum_required(VERSION 2.8.11)
project(${AppName})

set(AppSourcesDir src)
set(AppHeadersDir include)

# deps headers
set(CommonLibHeadersDir ../common/include)
set(GumboLibHeadersDir ../gumbo)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppHeadersList "include/*.h")
file(GLOB_RECURSE AppFormsList "ui/*.ui")
file(GLOB_RECURSE AppResourcesList "rcc/*.qrc")

qt5_wrap_ui(AppFormsHeadersList ${AppFormsList})
qt5_wrap_cpp(AppHeadersMocList ${AppFormsHeadersList})
qt5_add_resources(AppResourcesListAdded ${AppResourcesList})

list(REMOVE_ITEM AppSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" AppSourcesList)

source_group(
	generated_files FILES 
	${AppHeadersMocList} 
	${AppResourcesListAdded} 
	${AppResourcesList} 
	${AppFormsHeadersList}
)

source_group(precompiled_headers FILES include/stdafx.h src/stdafx.cpp)
source_group(ui FILES ${AppFormsList})
source_group(qrc FILES ${AppResourcesList})

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
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
		VS_DEBUGGER_WORKING_DIRECTORY $ENV{CMAKE_PREFIX_PATH}/bin
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES WIN32_EXECUTABLE YES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${AppHeadersDir}
	${CommonLibHeadersDir}
	${GumboLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${AppName}
	${CommonLibName}
	${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
	${Boost_REGEX_LIBRARY}
	Qt5::Widgets
	Qt5::Network
)