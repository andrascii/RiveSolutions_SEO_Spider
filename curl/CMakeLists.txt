cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)

include(ExternalProject)

externalproject_add(curl
    URL ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/curl-7.62.0.tar.gz
    PREFIX curl
    CMAKE_ARGS
        -DCURL_STATICLIB=ON
        -DCMAKE_DEBUG_POSTFIX=
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_CURL_EXE=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_USE_OPENSSL=ON
        -DCURL_ZLIB=OFF
        -DHTTP_ONLY=ON
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

externalproject_get_property(curl Source_Dir Binary_Dir)
set(CurlHeadersDir "${Source_Dir}/include" CACHE INTERNAL "")
include_directories(${CurlHeadersDir})
set(CurlDependency curl CACHE INTERNAL "")

set(CurlDebugDir ${Binary_Dir}/lib/debug)
set(CurlReleaseDir ${Binary_Dir}/lib/release)
message(STATUS "CURL Debug directory ${CurlDebugDir}")
message(STATUS "CURL Release directory ${CurlReleaseDir}")

if(APPLE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # in Debug mode
        set(CurlLibraries ${CurlDebugDir}/libcurl.a CACHE INTERNAL "")
    else (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # in Release mode
        set(CurlLibraries ${CurlReleaseDir}/libcurl.a CACHE INTERNAL "")
    endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
    list(APPEND CurlLibrariesList wsock32 ws2_32)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # in Debug mode
        set(CurlLibraries ${CurlLibrariesList} ${CurlDebugDir}/libcurl.lib CACHE INTERNAL "")
    else (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # in Release mode
        set(CurlLibraries ${CurlLibrariesList} ${CurlReleaseDir}/libcurl.lib CACHE INTERNAL "")
    endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
endif()

add_definitions(-DCURL_STATICLIB)
