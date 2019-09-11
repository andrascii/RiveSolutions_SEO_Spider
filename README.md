# RiveSolutions SEO Spider

# Сборка инсталлера

Для компилирования инсталлера надо загрузить и установить Qt Intaller Framework https://blog.qt.io/blog/2017/12/15/qt-installer-framework-3-0-2-released/

Затем задать новую системную переменную окружения `IFW_BIN_DIR = *\Qt Installer Framework 3.0.2\bin`

# Сборка под OS Windows

Для того, чтобы успешно сгенерировать и забилдить проект, надо:
1. Скачать deps.7z отсюда https://yadi.sk/d/I4qfKFy03TFN4g
1. Распаковать архив в корень диска `D` так, чтобы все зависимости лежали по пути `D:\\deps\`
1. Удалить все переменные среды `BOOST*`, `CMAKE_PREFIX_PATH`, `QT*`, а так же пути к ним в `path`
1. Создать `СИСТЕМНУЮ` переменную окружения SS_DEPS со значением d:\deps
1. `OPENSSL_ROOT_DIR` со значением `%SS_DEPS%\openssl\openssl_%SS_PLATFORM%`
1. `OPENSSL_INCLUDE_DIR` со значением `%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\include`
1. `IFW_BIN_DIR` со значением `%SS_DEPS%\IFW\%SS_PLATFORM%\bin`
1. Добавить в PATH значения:
- `%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin`
- `%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms`
- `%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib`
- `%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin`
- путь до cmake `C:\Program Files\CMake\bin` (но он может быть и другим, главное, чтобы путь до него был прописан в `path`)
1. Установить `СИСТЕМНУЮ` переменную окружения `SS_PLATFORM` правильно (x64 или win32).
1. Запустить `generate_solution_win32.bat` либо `generate_solution_x64.bat` в зависимости от желаемой сборки

Если нет диска D:, то нужно создать виртуальный диск: http://composs.ru/windows-10-vhd/ (необходимо примерно 20Гб)

# Deploy

Для создания готового инсталлера нужно вызвать билд скрипт `build_solution_win32.bat` или `build_solution_x64.bat`.

После сборки откроется `explorer.exe` с папкой билда.

В этом билде будет лежать три папки:
1. `config`
2. `packages`
3. `repository`

Также рядом будут находиться два установщика:
1. `seospiderinstaller-offline.exe`
2. `seospiderinstaller-online.exe`

Соответственно это оффлайн и онлайн версия инсталляторов.

# Загрузка новых билдов на наш сервер для обновления по воздуху

1. Заходим на наш хостинг на сайте `jino.ru`
2. Переходим в папку domains, затем в папку `rivesolutions.com`
3. В `rivesolutions.com` будет лежать папка `downloads`, заходим туда
4. В папке `downloads` будут лежать папки с именами ОС, выбираем `win`
5. В папке `win` будут находиться еще две папки - `х64` и `х86`
6. Заходим в ту, которая нам нужна, например в `х64`
7. Из директории с полученным билдом (описанным выше) заходим в папку `repository`, берем оттуда файл `Updates.xml`
	и заливаем его в корень нашей `х64` папки (это сигнал для скачивания апдейта).
	Именно этот файл опрашивается при запуске программы, если версия, указанная в этом файле выше версии программы, то будет показан диалог с предложением загрузки апдейта.
8. Далее, берем папку `repository/com.rivesolution.seospider` и заливаем ее на наш хостинг по пути `domains/rivesolutions.com/downloads/win/x64`
9. Последнее действие, берем exe инсталляторы из папки с билдом и заливаем их по пути `domains/rivesolutions.com/downloads/win/x64/dist` (папки `win` и `x64` зависят от типа сборки и ОС для которой билды предназначены).

## Использование FTP

Все эти действия для удобства можно выполнить через FTP подключение.
О том как подключиться к нашему хостингу можно прочитать в `README.md` файле в этом репозитории: https://github.com/andrascii/RiveSolutions_WordPress.git

# Сборка под Mac OS

## Для начала нужно установить зависимости

### Boost

На данном этапе мы вынуждены использовать кастомную сборку библиотеки `boost`.
А именно, в составе библиотеки `boost` должна идти библиотека `container_hash` из ветки `develop`.
Вот как выглядит процесс сборки:

1. `git clone https://github.com/boostorg/boost.git`
2. `cd boost`
3. `git submodule update --init`
4. `cd libs/container_hash`
5. `git checkout develop`
6. `cd -`
7. `./bootstrap.sh`
8. `./b2`
9. `./b2 install`

После этого мы получим собранную и установленную в систему библиотеку `boost`.

О том как удалить из системы библиотеку `boost`, собранную из исходников: https://askubuntu.com/questions/325504/ubuntu-12-04-uninstall-boost-installed-from-source

### Qt

`brew install qt`

### cURL

`brew install curl`

## Генерация проекта для Xcode

1. `git clone https://github.com/andrascii/RiveSolutions_SEO_Spider.git`
2. `git checkout macos`
3. `cmake -B build -G Xcode`
4. Открыть проект с помощью `Xcode` в папке `build`

# Сборка под OS Linux

Linux говно и сборок под него никогда не будет!
