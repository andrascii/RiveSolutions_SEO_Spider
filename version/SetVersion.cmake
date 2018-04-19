set(SRC_DIR "E:/Develop/SourceTree/quickiewebbot/version")
set(versionHeaderPath "E:/Develop/SourceTree/quickiewebbot/version/include")
set(installerConfigPath "E:/Develop/SourceTree/quickiewebbot/version/../seospiderinstaller/packages/com.rivesolutions.seospider/meta")

set(MAJOR 2)
set(MINOR 1)
set(MAINTANCE 0)

configure_file(
	${versionHeaderPath}/version.h.in 
	${versionHeaderPath}/version.h
)


configure_file(
	${installerConfigPath}/package.xml.in 
	${installerConfigPath}/package.xml
)

message("Version set") # For testing only
