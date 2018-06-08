# RiveSolutions SEO Spider

# Installer

Для компилирования инсталлера надо загрузить и установить Qt Intaller Framework https://blog.qt.io/blog/2017/12/15/qt-installer-framework-3-0-2-released/
Затем задать новую системную переменную окружения IFW_BIN_DIR = *\Qt Installer Framework 3.0.2\bin

# Build steps

Для того, чтобы успешно сгенерировать и забилдить проект, надо:
1. Скачать deps.7z отсюда https://yadi.sk/d/I4qfKFy03TFN4g
2. Распаковать архив в корень диска D так, чтобы все зависимости лежали по пути D:\\deps\
3. Удалить все переменные среды BOOST*, CMAKE_PREFIX_PATH, QT*, а так же пути к ним в path
4. Создать переменную среды SS_DEPS со значением d:\deps
5. Добавить в PATH значения:
	%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin
	%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms
	%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib
	%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin
	путь до cmake (C:\Program Files\CMake\bin)
6. Установить переменную среды SS_PLATFORM правильно (x64 или win32).
7. Запустить generate_solution_win32.bat либо generate_solution_x64.bat

Если нет диска D:, то нужно создать виртуальный диск: http://composs.ru/windows-10-vhd/ (необходимо примерно 20Гб)
