cd ..
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
mkdir build
cd build
del CMakeCache.txt
set SS_PLATFORM=x64
set CMAKE_PREFIX_PATH=%SS_DEPS%\msvc_2017_%SS_PLATFORM%
set BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib
set BOOST_ROOT=%SS_DEPS%\boost
set path=%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
set TESTS=disabled
cmake -G "Visual Studio 15 2017 Win64" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Release
msbuild.exe .\seospiderinstaller\seospiderinstaller.vcxproj /t:Build /p:Configuration=Release;Platform=x64
cd ..
cd ./buildscripts/