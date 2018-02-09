cmake_minimum_required(VERSION 2.8.11)
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

set(QTIFW_BIN_DIR $ENV{IFW_BIN_DIR})
find_program(BinaryCreator_executable binarycreator HINTS ${QTIFW_BIN_DIR})

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

#add_custom_target(${SeoSpiderInstallerName} ALL)
add_custom_target(${SeoSpiderInstallerName})

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
    COMMAND ${CMAKE_COMMAND} -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
          --release --no-translations "${TargetPackagePath}/data/"
    COMMENT "Running windeployqt..."
)

message(${BinaryCreator_executable})

if(${BinaryCreator_executable})

message(${BinaryCreator_executable})

elseif(NOT ${BinaryCreator_executable})
	add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND "${BinaryCreator_executable}"
           -f -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller.exe
    COMMENT "Running binarycreator..."
	)
	
endif()

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND start explorer.exe /n,/e,/select, ${TargetInstallerPath}\\seospiderinstaller.exe
)