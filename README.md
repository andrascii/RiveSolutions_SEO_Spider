# RiveSolutions SEO Spider

# Installer

Для компилирования инсталлера надо загрузить и установить Qt Intaller Framework https://blog.qt.io/blog/2017/12/15/qt-installer-framework-3-0-2-released/

Затем задать новую системную переменную окружения `IFW_BIN_DIR = *\Qt Installer Framework 3.0.2\bin`

# Build steps

Для того, чтобы успешно сгенерировать и забилдить проект, надо:
1. Скачать deps.7z отсюда https://yadi.sk/d/I4qfKFy03TFN4g
1. Распаковать архив в корень диска `D` так, чтобы все зависимости лежали по пути `D:\\deps\`
1. Создать `СИСТЕМНУЮ` переменную окружения `SS_DEPS` со значением `d:\deps`
1. Добавить следующие переменные окружения в систему:
- `BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib`
- `BOOST_ROOT=%SS_DEPS%\boost`
- `CMAKE_PREFIX_PATH=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%`
- `OPENSSL_ROOT_DIR=%SS_DEPS%\openssl\openssl_%SS_PLATFORM%`
- `OPENSSL_INCLUDE_DIR=%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\include`
- `IFW_BIN_DIR=%SS_DEPS%\IFW\%SS_PLATFORM%\bin`
1. Добавить в PATH значения:
- `%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin`
- `%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms`
- `%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib`
- `%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin`
- путь до cmake `C:\Program Files\CMake\bin` (но он может быть и другим, главное, чтобы путь до него был прописан в `path`)
1. Установить `СИСТЕМНУЮ` переменную окружения `SS_PLATFORM` правильно (x64 или win32).
1. Запустить `vs2017_win32.bat` либо `vs2017_x64.bat` в зависимости от желаемой сборки

Если нет диска D:, то нужно создать виртуальный диск: http://composs.ru/windows-10-vhd/ (необходимо примерно 20Гб)

# Deploy

Для создания готового инсталлера нужно вызвать билд скрипт build_solution_win32.bat или build_solution_x64.bat.

После сборки откроется explorer.exe с папкой билда.

В этом билде будет лежать три папки:
1. config
2. packages
3. repository

Также рядом будут находиться два установщика:
1. seospiderinstaller-offline.exe
2. seospiderinstaller-online.exe

Соответственно это оффлайн и онлайн версия инсталляторов.

# Uploading builds to hosting

1. Заходим на наш хостинг на сайте jino.ru
2. Переходим в папку domains, затем в папку rivesolutions.com
3. В rivesolutions.com будет лежать папка downloads, заходим туда
4. В папке downloads будут лежать папки с именами ОС, выбираем win
5. В папке win будут находиться еще две папки - х64 и х86
6. Заходим в ту, которая нам нужна, например в х64
7. Из директории с полученным билдом (описанным выше) заходим в папку repository, берем оттуда файл Updates.xml 
	и заливаем его в корень нашей х64 папки (это сигнал для скачивания апдейта).
	Именно этот файл опрашивается при запуске программы, если версия, указанная в этом файле выше версии программы, то будет показан диалог с предложением загрузки апдейта.
8. Далее, берем папку ./repository/com.rivesolution.seospider и заливаем ее на наш хостинг по пути /domains/rivesolutions.com/downloads/win/x64
9. Последнее действие, берем exe инсталляторы из папки с билдом и заливаем их по пути /domains/rivesolutions.com/downloads/win/x64/dist (папки win и x64 зависят от типа сборки и ОС для которой билды предназначены).

## Using FTP

Все эти действия для удобства можно выполнить через FTP подключение.
О том как подключиться к нашему хостингу можно прочитать в `README.md` файле в этом репозитории: https://github.com/andrascii/RiveSolutions_WordPress.git
