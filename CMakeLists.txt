cmake_minimum_required(VERSION 2.8.11)

set(AppName quickiewebbot)
set(LoggerAppName logger)
set(WebCrawlerLibName webcrawler)
project(${AppName})

set(GuiBinName ${AppName})
set(LoggerBinName ${LoggerAppName})
set(GumboLibName gumbo)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Widgets Network Svg COMPONENTS)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system date_time regex COMPONENTS)

MACRO(add_msvc_precompiled_header PrecompiledHeader PrecompiledSource SourcesVar)
if(MSVC)
	get_filename_component(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    set(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    set(Sources ${${SourcesVar}})
	
    set_source_files_properties(
		${PrecompiledSource}
        PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_OUTPUTS "${PrecompiledBinary}"
	)
	
	set_source_files_properties(
		${Sources}
		PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_DEPENDS "${PrecompiledBinary}"
	)  
	
	list(APPEND ${SourcesVar} ${PrecompiledSource})
	
endif(MSVC)
ENDMACRO(add_msvc_precompiled_header)

add_subdirectory(${WebCrawlerLibName})
add_subdirectory(${LoggerAppName})
add_subdirectory(${GuiBinName})
add_subdirectory(${GumboLibName})
