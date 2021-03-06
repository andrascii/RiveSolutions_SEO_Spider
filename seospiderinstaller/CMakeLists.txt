cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
project(${SeoSpiderInstallerName})

set(ReleasePackageDir ../seospider/Release)
set(PackageFileList)
set(TargetInstallerPath installer)
set(TargetPackagePath ${TargetInstallerPath}/packages/com.rivesolutions.seospider)

set(InstallerConfigDir ../../seospiderinstaller/config)
set(PackageMetaDir ../../seospiderinstaller/packages/com.rivesolutions.seospider/meta)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

message("qmake path: ${_qmake_executable}")
message("Qt binary directory: ${_qt_bin_dir}")
message("windeployqt utility path: ${WINDEPLOYQT_EXECUTABLE}")

set(QTIFW_BIN_DIR $ENV{IFW_BIN_DIR})
find_program(BinaryCreator_executable binarycreator HINTS ${QTIFW_BIN_DIR})
find_program(RepoGen_executable repogen HINTS ${QTIFW_BIN_DIR})

#message(${QTIFW_BIN_DIR})

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

#add_custom_target(${SeoSpiderInstallerName} ALL)
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

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory
	${PackageMetaDir}
	${TargetPackagePath}/meta/
)

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
          --release --no-translations --compiler-runtime "${TargetPackagePath}/data/"
    COMMENT "Running windeployqt..."
)

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


if(${RepoGen_executable})
	
elseif(NOT ${RepoGen_executable})
	add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND "${RepoGen_executable}"
           -p ${TargetInstallerPath}/packages --update ${TargetInstallerPath}/repository
    COMMENT "Running repogen..."
	)
endif()

if(${BinaryCreator_executable})

elseif(NOT ${BinaryCreator_executable})
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

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND start explorer.exe /n,/e,/select, ${TargetInstallerPath}\\seospiderinstaller-offline.exe
)