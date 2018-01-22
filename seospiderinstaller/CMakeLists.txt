cmake_minimum_required(VERSION 2.8.11)
project(${SeoSpiderInstallerName})

set(ReleasePackageDir ../seospider/Release)
set(PackageFileList)
set(TargetInstallerPath installer)
set(TargetPackagePath ${TargetInstallerPath}/packages/com.rivesolutions.seospider)

set(InstallerConfigFile ../../seospiderinstaller/config/config.xml)
set(PackageConfigFile ../../seospiderinstaller/packages/com.rivesolutions.seospider/meta/package.xml)
set(PackageLicenseFile ../../seospiderinstaller/packages/com.rivesolutions.seospider/meta/license.txt)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

find_program(BinaryCreator_executable binarycreator HINTS "${_qt_bin_dir}/../../")
get_filename_component(BinaryCreator_dir ${BinaryCreator_executable} DIRECTORY)

list(APPEND PackageFileList 
		${ReleasePackageDir}/seospider.exe
		${ReleasePackageDir}/seospider.pdb
		${ReleasePackageDir}/seospiderservice.exe
		${ReleasePackageDir}/seospiderservice.pdb
		${ReleasePackageDir}/seospiderserviceapi.dll
		${ReleasePackageDir}/seospiderserviceapi.pdb
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
	ARGS -E copy 
	${InstallerConfigFile}
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
	ARGS -E copy 
	${PackageConfigFile}
	${TargetPackagePath}/meta/
)

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy 
	${PackageLicenseFile}
	${TargetPackagePath}/meta/
)

add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
           "${TargetPackagePath}/data/"
    COMMENT "Running windeployqt..."
)

if(NOT(${BinaryCreator_executable} EQUAL <VAR>-NOTFOUND))
message(${BinaryCreator_executable})
message(${BinaryCreator_dir})

	add_custom_command(
	TARGET ${SeoSpiderInstallerName} 
	POST_BUILD
    COMMAND "${BinaryCreator_executable}"
           -f -c ${TargetInstallerPath}/config/config.xml -p ${TargetInstallerPath}/packages ${TargetInstallerPath}/seospiderinstaller.exe
    COMMENT "Running binarycreator..."
	)
endif()