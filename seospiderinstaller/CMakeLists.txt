cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
project(${SeoSpiderInstallerName})

set(ReleasePackageDir ${CMAKE_CURRENT_BINARY_DIR}/../seospider/${CMAKE_BUILD_TYPE})
set(PackageFileList)
set(TargetInstallerPath installer)
set(TargetPackagePath ${TargetInstallerPath}/packages/com.rivesolutions.seospider)

set(InstallerConfigDir ../../seospiderinstaller/config)
set(PackageMetaDir ../../seospiderinstaller/packages/com.rivesolutions.seospider/meta)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if(APPLE)
	find_program(DEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
elseif(WIN32)
	find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
endif()

message("qmake path: ${_qmake_executable}")
message("Qt binary directory: ${_qt_bin_dir}")
message("deployqt utility path: ${DEPLOYQT_EXECUTABLE}")

set(QTIFW_BIN_DIR $ENV{IFW_BIN_DIR})
find_program(BinaryCreator_executable binarycreator HINTS ${QTIFW_BIN_DIR})
find_program(RepoGen_executable repogen HINTS ${QTIFW_BIN_DIR})

message(${QTIFW_BIN_DIR})

# this ${PackageFileList} used to copy files only on OS Windows
# on MacOS we copy seospider.app bundle whish is actually a directory
# so it should be copied another way (see below under "files copying block")
list(APPEND PackageFileList 
	${ReleasePackageDir}/seospider.exe
	${ReleasePackageDir}/seospider.pdb
	${ReleasePackageDir}/seospiderservice.exe
	${ReleasePackageDir}/seospiderservice.pdb
	${ReleasePackageDir}/seospiderserviceapi.dll
	${ReleasePackageDir}/seospiderserviceapi.pdb
	${ReleasePackageDir}/QtXlsxWriter.dll
	${ReleasePackageDir}/QtXlsxWriter.pdb
)

if(${ENABLE_SCREENSHOTS})
	list(APPEND PackageFileList 
		${ReleasePackageDir}/screenshotmaker.exe
		${ReleasePackageDir}/screenshotmaker.pdb
	)
endif()

file(GLOB_RECURSE OpenSslDllList "$ENV{SS_DEPS}/openssl/openssl_$ENV{SS_PLATFORM}/bin/*.dll")

if("$ENV{SS_PLATFORM}" STREQUAL "win32")
	set(ApplicationsDir @ApplicationsDirX86@)	
elseif("$ENV{SS_PLATFORM}" STREQUAL "x64")
	set(ApplicationsDir @ApplicationsDirX64@)
endif()	

set(TargetDir @TargetDir@)

configure_file(
	config/config.xml.in 
	${InstallerConfigDir}/config.xml
)

add_custom_target(${SeoSpiderInstallerName})

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E remove_directory 
	${TargetPackagePath}/data
)

######### dirs creation block ##################
add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E make_directory 
	${TargetPackagePath}/data
)

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E make_directory 
	${TargetPackagePath}/meta
)

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E make_directory 
	${TargetInstallerPath}/config
)

######### files copying block ######################
add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory
	${InstallerConfigDir}
	${TargetInstallerPath}/config
)

message(WARNING "Possibly this build will not support HTTPS protocol because we does not deploy OpenSSL library!")

if(APPLE)
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E make_directory
		${TargetPackagePath}/data/seospider.app
	)

	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy_directory
		${ReleasePackageDir}/seospider.app
		${TargetPackagePath}/data/seospider.app
	)
else()
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
		${OpenSslDllList}
		${TargetPackagePath}/data/
	)

	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
		${VMProctectSdkDll}
		${TargetPackagePath}/data/
	)

	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
		${PackageFileList}
		${TargetPackagePath}/data/
	)
endif()

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory
	${PackageMetaDir}
	${TargetPackagePath}/meta/
)

if(APPLE)
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND "${DEPLOYQT_EXECUTABLE}"
			"${TargetPackagePath}/data/seospider.app"
		COMMENT "Running deployqt..."
	)
else()
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND "${DEPLOYQT_EXECUTABLE}"
			--release --no-translations --compiler-runtime "${TargetPackagePath}/data/"
		COMMENT "Running deployqt..."
	)
endif()

if (NOT APPLE)
	if("$ENV{SS_PLATFORM}" STREQUAL "win32")
		set(vc_redist "vc_redist.x86.exe")	
	elseif("$ENV{SS_PLATFORM}" STREQUAL "x64")
		set(vc_redist "vc_redist.x64.exe")
	endif()	

	add_custom_command(
		TARGET ${SeoSpiderInstallerName}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E rename 
		${TargetPackagePath}/data/${vc_redist}
		${TargetPackagePath}/data/vcredist.exe
	)
endif()

if(NOT ${RepoGen_executable})
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND "${RepoGen_executable}"
			-p ${TargetInstallerPath}/packages --update ${TargetInstallerPath}/repository
		COMMENT "Running repogen..."
	)
endif()

if(NOT ${BinaryCreator_executable})
	if(APPLE)
		add_custom_command(
			TARGET ${SeoSpiderInstallerName} 
			POST_BUILD
			COMMAND "${BinaryCreator_executable}"
				-f -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller-offline.dmg
			COMMENT "Running binarycreator..."
		)	
		
		add_custom_command(
			TARGET ${SeoSpiderInstallerName} 
			POST_BUILD
			COMMAND "${BinaryCreator_executable}"
				-n -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller-online.dmg
			COMMENT "Running binarycreator..."
		)
	else()
		add_custom_command(
			TARGET ${SeoSpiderInstallerName} 
			POST_BUILD
			COMMAND "${BinaryCreator_executable}"
				-f -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller-offline.exe
			COMMENT "Running binarycreator..."
		)	
		
		add_custom_command(
			TARGET ${SeoSpiderInstallerName} 
			POST_BUILD
			COMMAND "${BinaryCreator_executable}"
				-n -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller-online.exe
			COMMENT "Running binarycreator..."
		)
	endif()
endif()

if(NOT APPLE)
	add_custom_command(
		TARGET ${SeoSpiderInstallerName} 
		POST_BUILD
		COMMAND start explorer.exe /n,/e,/select, ${TargetInstallerPath}\\seospiderinstaller-offline.exe
	)
endif()