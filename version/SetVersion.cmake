set(SRC_DIR "E:/develop/repositories/quickiewebbot/version")
set(versionHeaderPath "E:/develop/repositories/quickiewebbot/version/include")
set(installerConfigPath "E:/develop/repositories/quickiewebbot/version/../seospiderinstaller/packages/com.rivesolutions.seospider/meta")

set(MAJOR 1)
set(MINOR 0)
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
