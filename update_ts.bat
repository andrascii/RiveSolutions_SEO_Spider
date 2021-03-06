@echo Updating translation files (.ts) via LUPDATE.EXE

@set BAT_ROOT_DIR=%CD%
@set SOURCE_DIR=%CD%\seospider\src
@set HEADERS_DIR=%CD%\seospider\include\
@set COMMON_SOURCE_DIR=%CD%\common\src
@set COMMON_HEADERS_DIR=%CD%\common\include\
@set CRAWLERENGINE_SOURCE_DIR=%CD%\crawlerengine\src
@set CRAWLERENGINE_HEADERS_DIR=%CD%\crawlerengine\include\
@set TRANSLATIONS_DIR=%CD%\seospider\rcc\translations\
@set USER_ITERFACE_DIR=%CD%\seospider\ui
@set LUPDATE_EXECUTABELE=D:\deps\qt\msvc_2017_x64\bin\lupdate.exe



@IF EXIST %LUPDATE_EXECUTABELE% ( 
@echo lupdate.exe founded at %LUPDATE_EXECUTABELE%
) ELSE ( 
@echo lupdate.exe not found. Be sure that CMAKE_PREFIX_PATH is setted to compiler directory ("qt5/msvc2017" for example) 
)
@echo.

@echo Source directory: %SOURCE_DIR%
@echo Headers directory: %HEADERS_DIR%
@echo User interface directory: %USER_ITERFACE_DIR%
@echo Translations directory: %TRANSLATIONS_DIR%
@echo.

@cd %TRANSLATIONS_DIR%
@for %%i in (*.ts) do (
@echo %%i 
start /B %LUPDATE_EXECUTABELE% %SOURCE_DIR% %HEADERS_DIR% %USER_ITERFACE_DIR% %COMMON_SOURCE_DIR% %COMMON_HEADERS_DIR% %CRAWLERENGINE_SOURCE_DIR% %CRAWLERENGINE_HEADERS_DIR% -ts %%i
)

@cd %BAT_ROOT_DIR%

@pause

