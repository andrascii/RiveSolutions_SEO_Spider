cmake_minimum_required(VERSION 2.8.11)

project(${GumboLibName})

aux_source_directory(./ GumboSources)
file(GLOB_RECURSE GumboHeaders "./*.h")

configure_msvc_runtime()
add_library(
	${GumboLibName} STATIC 
	${GumboSources} 
	${GumboHeaders}
)

if(MSVC)

	include_directories(include/)
	
	add_definitions(
		-Dinline=__inline
		/MP
		/permissive-
		/Zi
	)
	
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi /GL")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS} /LTCG")
	
	set_target_properties(
		${GumboLibName}
		PROPERTIES LINK_FLAGS /DEBUG
	)
	
endif()