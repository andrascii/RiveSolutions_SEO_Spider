call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Buildvcvars32.bat"
mkdir build
cd build
del CMakeCache.txt
set SS_PLATFORM=win32
set CMAKE_PREFIX_PATH=%SS_DEPS%\msvc_2017_%SS_PLATFORM%
set BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib
set BOOST_ROOT=%SS_DEPS%\boost
set path=%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
cmake -G "Visual Studio 15 2017" .. -DMSVC_RUNTIME=dynamic