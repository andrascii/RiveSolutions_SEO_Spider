call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
D:
cd /
mkdir deps
cd deps
mkdir build
cd build
if not exist qt5 (
	git clone https://github.com/qt/qt5.git
	)

cd qt5
git fetch
git checkout 5.13
perl init-repository -f
git clean -dfx
git submodule foreach "git clean -dfx"
set path=c:\jom;%path%
perl init-repository
call configure.bat -confirm-license -prefix D:\deps\qt\msvc_2017_win32_static_base -release -opensource -static -static-runtime -accessibility -no-opengl -no-icu -no-sql-sqlite -no-qml-debug -nomake examples -nomake tests -skip qtactiveqt -skip qtlocation -skip qtmultimedia -skip qtserialport -skip qtquickcontrols -skip qtscript -skip qtsensors -skip qtwebsockets -skip qtxmlpatterns -skip qt3d -skip qtwebview -skip qtwebengine -skip qtquickcontrols -skip qtquickcontrols2 -skip qtpurchasing
jom
jom install
git clean -dfx
git submodule foreach "git clean -dfx"

set path=D:\deps\qt\msvc_2017_win32_static_base\bin;%path%

cd /
cd deps
mkdir IFW
cd IFW
if not exist win32 (
	git clone https://github.com/qtproject/installer-framework.git win32
	)

cd win32
git clean -dfx
qmake
nmake
nmake install
