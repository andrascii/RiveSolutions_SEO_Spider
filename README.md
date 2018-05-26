# QuickieWebBot
# Installing

All CMakeLists.txt files intended for creating project for Visual Studio.

Note:

For successful build and generating Visual Studio project you need boost library at least 1.64.0 version.

For compiling installer you should Downoad and Setup Qt Intaller Framework https://blog.qt.io/blog/2017/12/15/qt-installer-framework-3-0-2-released/
and set new system variable IFW_BIN_DIR to *\Qt Installer Framework 3.0.2\bin

Steps for build:

1. Положить депсы в d:\deps
2. Удалить все переменные среды BOOST*, CMAKE_PREFIX_PATH, QT*, а так же пути к ним в path
3. Создать переменную среды SS_DEPS со значением d:\deps
4. Добавить в PATH значения:
	%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin
	%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms
	%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib
	%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin
	путь до cmake (C:\Program Files\CMake\bin)
4. Установить переменную среды SS_PLATFORM правильно (x64 или win32).
6. Запустить generate_solution_win32.bat либо generate_solution_x64.bat

Если нет диска D:, то нужно создать виртуальный диск: http://composs.ru/windows-10-vhd/ (необходимо примерно 20Гб)
