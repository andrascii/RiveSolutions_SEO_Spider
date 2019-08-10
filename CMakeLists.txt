cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)

macro(configure_msvc_runtime)
  if(MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
      set(MSVC_RUNTIME "static")
    endif()
    # Set compiler options.
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(${MSVC_RUNTIME} STREQUAL "static")
      message(STATUS
        "MSVC -> forcing use of statically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
      endforeach()
    else()
      message(STATUS
        "MSVC -> forcing use of dynamically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
        endif()
      endforeach()
    endif()
  endif()
endmacro()

set(AppName seospider)
set(ScreenshotMaker screenshotmaker)
set(CrawlerEngineLibName crawlerengine)
set(GoogleTestLibName googletest/googletest)
set(CrawlerEngineLibTestsName crawlerenginetests)
set(CommonLibName common)
set(QxtSmtpLibName qxtsmtp)
set(QUazipLibName quazip)
set(ZLibName zlib)
set(GumboLibName gumbo)
set(MyHtmlLibName myhtml)
set(SeoSpiderServiceApiDllName seospiderserviceapi)
set(SeoSpiderServiceAppName seospiderservice)
set(QtXlsxWriter qtxlsxwriter)
set(SeoSpiderInstallerName seospiderinstaller)
set(VersionProjectName version)

if(WIN32)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/windows")

  if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(VMProctectSdkDll "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/windows/VMProtectSDK32.dll")
  else()
    set(VMProctectSdkDll "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/windows/VMProtectSDK64.dll")
  endif()

endif()

if(UNIX AND NOT APPLE)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/linux")
endif()

if(APPLE)
  set(VMProtectSdkLibDir "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/osx")
  set(VMProctectSdkDll "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/bin/osx/libVMProtectSDK.dylib")
endif()

set(VMProtectSdkHeadersDir ../thirdparty/vmprotect/include)
set(VMProtectDebugLicenseFile "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/vmprotect/debuglicense/VMProtectLicense.ini")

project(${AppName})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(CMAKE_INSTALL_PREFIX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(QtXlsxWriter_DIR ${CMAKE_CURRENT_BINARY_DIR}/${QtXlsxWriter})

find_package(Qt5 REQUIRED PrintSupport Widgets Network Svg WebEngineWidgets Xml XmlPatterns COMPONENTS)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system date_time program_options filesystem COMPONENTS)
message("Boost found ${Boost_FOUND}")

if(APPLE)
  find_package(OpenSSL)
  find_package(curl)
  message("OpenSSL found ${OPENSSL_FOUND}")
  message("CURL found ${OPENSSL_FOUND}")
endif()

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

if(NOT APPLE)
  add_subdirectory(curl)
endif()

add_subdirectory(${CrawlerEngineLibName})

if ($ENV{TESTS} and $ENV{TESTS} STREQUAL "enabled")
	add_subdirectory(${CrawlerEngineLibTestsName})	
endif()

add_subdirectory(${QtXlsxWriter})
add_subdirectory(${AppName})
#add_subdirectory(${ScreenshotMaker}) # disabled due first building on Mac
add_subdirectory(${GumboLibName})
add_subdirectory(${MyHtmlLibName})
#add_subdirectory(${SeoSpiderServiceApiDllName}) # disabled due first building on Mac
#add_subdirectory(${SeoSpiderServiceAppName}) # disabled due first building on Mac
add_subdirectory(${CommonLibName})
add_subdirectory(${QxtSmtpLibName})
add_subdirectory(${ZLibName})
add_subdirectory(${QUazipLibName})
#add_subdirectory(${GoogleTestLibName}) # disabled due first building on Mac
#add_subdirectory(${SeoSpiderInstallerName}) # disabled due first building on Mac
add_subdirectory(${VersionProjectName})