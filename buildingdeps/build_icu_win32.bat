D:
cd D:\deps\build\icu\source
set PATH=%PATH%;C:\Cygwin64\bin
dos2unix *
dos2unix -f configure
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
bash runConfigureICU Cygwin/MSVC -prefix=/cygdrive/d/deps/icu/icu_win32
make && make install