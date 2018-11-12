cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

include(ExternalProject)

externalproject_add(curl
	URL https://curl.haxx.se/download/curl-7.62.0.tar.gz
	PREFIX curl
	CMAKE_ARGS -DCURL_STATICLIB=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCMAKE_USE_OPENSSL=ON -DCURL_ZLIB=OFF -DHTTP_ONLY=ON
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(curl Source_Dir Binary_Dir)
include_directories("${Source_Dir}/include")
list(APPEND CurlLibraries wsock32 ws2_32)
set(CurlDependency curl)

set(CurlDebugDir ${Binary_Dir}/lib/debug)
set(CurlReleaseDir ${Binary_Dir}/lib/release)
message(STATUS "CURL Debug directory ${CurlDebugDir}")
message(STATUS "CURL Release directory ${CurlReleaseDir}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	# in Debug mode
	list(APPEND CurlLibraries ${CurlDebugDir}/libcurl-d.lib)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
	# in Release mode
	list(APPEND CurlLibraries ${CurlReleaseDir}/libcurl.lib)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_definitions(-DCURL_STATICLIB)