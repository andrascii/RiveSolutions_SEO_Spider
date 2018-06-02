call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
D:
cd /
mkdir deps
cd deps
mkdir build
cd build
rmdir .\openssl /s /q
git clone https://github.com/openssl/openssl.git
cd openssl
git fetch
git checkout OpenSSL_1_1_0-stable
perl Configure VC-WIN32 --prefix=c:\openssl
nmake
nmake test
nmake install
nmake clear

xcopy /E /I c:\openssl d:\deps\openssl\openssl_win32

cd D:\deps\build
if not exist qt5 (
	git clone https://github.com/qt/qt5.git
	)

cd qt5
git fetch
git checkout 5.10
perl init-repository
git clean -dfx
git submodule foreach "git clean -dfx"
set path=c:\jom;%path%
perl init-repository
set path=D:\deps\icu\icu_win32\lib;%path%
call configure.bat -confirm-license -debug-and-release -opensource -shared -prefix D:\deps\qt\msvc_2017_win32 -mp -nomake examples -force-debug-info -I c:\\openssl\include -L c:\openssl\\lib OPENSSL_LIBS="-llibcrypto -llibssl -lgdi32" -openssl-linked -no-angle -opengl desktop -icu -I d:/deps/icu/icu_win32/include -L d:/deps/icu/icu_win32/lib -no-vulkan
jom
jom install
git clean -dfx
git submodule foreach "git clean -dfx"