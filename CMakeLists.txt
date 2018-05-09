cmake_minimum_required(VERSION 2.8.11)

set(AppName seospider)
set(CrawlerEngineLibName crawlerengine)
set(GoogleTestLibName googletest/googletest)
set(CrawlerEngineLibTestsName crawlerenginetests)
set(CommonLibName common)
set(QxtSmtpLibName qxtsmtp)
set(QUazipLibName quazip)
set(ZLibName zlib)
set(GumboLibName gumbo)
set(SeoSpiderServiceApiDllName seospiderserviceapi)
set(SeoSpiderServiceAppName seospiderservice)
set(QtXlsxWriter qtxlsxwriter)
set(SeoSpiderInstallerName seospiderinstaller)
set(VersionProjectName version)

if(WIN32)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/bin/windows")

  if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(VMProctectSdkDll "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/bin/windows/VMProtectSDK32.dll")
  else()
    set(VMProctectSdkDll "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/bin/windows/VMProtectSDK64.dll")
  endif()

endif()

if(UNIX AND NOT APPLE)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/bin/linux")
endif()

if(APPLE)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/bin/osx")
endif()

set(VMProtectDebugLicenseFile "${CMAKE_CURRENT_SOURCE_DIR}/deps/vmprotect/debuglicense/VMProtectLicense.ini")

project(${AppName})

set(GuiBinName ${AppName})

set(CMAKE_INSTALL_PREFIX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(QtXlsxWriter_DIR ${CMAKE_CURRENT_BINARY_DIR}/${QtXlsxWriter})

find_package(Qt5 REQUIRED PrintSupport Widgets Network Svg WebEngineWidgets Xml XmlPatterns COMPONENTS)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system date_time program_options COMPONENTS)

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

add_subdirectory(${CrawlerEngineLibName})
add_subdirectory(${CrawlerEngineLibTestsName})
add_subdirectory(${QtXlsxWriter})
add_subdirectory(${GuiBinName})
add_subdirectory(${GumboLibName})
add_subdirectory(${SeoSpiderServiceApiDllName})
add_subdirectory(${SeoSpiderServiceAppName})
add_subdirectory(${CommonLibName})
add_subdirectory(${QxtSmtpLibName})
add_subdirectory(${ZLibName})
add_subdirectory(${QUazipLibName})
add_subdirectory(${GoogleTestLibName})
add_subdirectory(${SeoSpiderInstallerName})
add_subdirectory(${VersionProjectName})