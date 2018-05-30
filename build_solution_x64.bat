call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Buildvcvars64.bat"
mkdir build
cd build
del CMakeCache.txt
set SS_PLATFORM=x64
set CMAKE_PREFIX_PATH=%SS_DEPS%\msvc_2017_%SS_PLATFORM%
set BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib
set BOOST_ROOT=%SS_DEPS%\boost
set path=%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
cmake -G "Visual Studio 15 2017 Win64" ..
msbuild.exe seospider.sln /t:Build /p:Configuration=Release;Platform=x64
msbuild.exe .\seospiderinstaller\seospiderinstaller.vcxproj /t:Build /p:Configuration=Release;Platform=x64