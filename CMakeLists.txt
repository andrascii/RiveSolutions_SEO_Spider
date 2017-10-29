cmake_minimum_required(VERSION 2.8.11)

set(AppName seospider)
set(LoggerAppName logger)
set(WebCrawlerLibName webcrawler)
set(GoogleTestLibName googletest/googletest)
set(WebCrawlerLibTestsName webcrawlertests)
set(CommonLibName common)
set(GumboLibName gumbo)
set(SeoSpiderServiceApiDllName seospiderserviceapi)
set(SeoSpiderServiceAppName seospiderservice)
project(${AppName})

set(CMAKE_INSTALL_PREFIX )

set(GuiBinName ${AppName})
set(LoggerBinName ${LoggerAppName})
set(WatchDogServiceName ${SeoSpiderServiceAppName})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Widgets Network Svg WebEngineWidgets COMPONENTS)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system date_time COMPONENTS)

MACRO(add_msvc_precompiled_header PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(
		${PrecompiledSource}
		PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
		OBJECT_OUTPUTS "${PrecompiledBinary}"
	)
	   
    SET_SOURCE_FILES_PROPERTIES(
		${Sources}
        PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_DEPENDS "${PrecompiledBinary}"
	)  
		
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(add_msvc_precompiled_header)

add_subdirectory(${WebCrawlerLibName})
add_subdirectory(${WebCrawlerLibTestsName})
add_subdirectory(${LoggerAppName})
add_subdirectory(${GuiBinName})
add_subdirectory(${GumboLibName})
add_subdirectory(${CommonLibName})
add_subdirectory(${SeoSpiderServiceApiDllName})
add_subdirectory(${SeoSpiderServiceAppName})
add_subdirectory(${GoogleTestLibName})
