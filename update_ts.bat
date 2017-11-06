@echo Updating translation files (.ts) via LUPDATE.EXE

@set BAT_ROOT_DIR=%CD%
@set SOURCE_DIR=%CD%\seospider\src
@set HEADERS_DIR=%CD%\seospider\include\
@set TRANSLATIONS_DIR=%CD%\seospider\rcc\translations\
@set USER_ITERFACE_DIR=%CD%\seospider\ui
@set LUPDATE_EXECUTABELE=%CMAKE_PREFIX_PATH%\bin\lupdate.exe



@IF EXIST %CMAKE_PREFIX_PATH%\bin\lupdate.exe ( 
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
start /B %LUPDATE_EXECUTABELE% %SOURCE_DIR% %HEADERS_DIR% %USER_ITERFACE_DIR% -ts %%i
)
@cd %BAT_ROOT_DIR%

@pause

