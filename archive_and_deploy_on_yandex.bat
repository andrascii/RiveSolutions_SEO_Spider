cd build\seospiderinstaller\installer
del installer.7z
7z a -t7z installer.7z *.* -r
xcopy /s /Y installer.7z %SS_YANDEX_DIR%\%SS_PLATFORM%