set SS_DEPS=D:\deps
set CMAKE_PREFIX_PATH=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%
set BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib
set BOOST_ROOT=%SS_DEPS%\boost
echo SS_DEPS: %SS_DEPS%
echo CMAKE_PREFIX_PATH: %CMAKE_PREFIX_PATH%
echo BOOST_LIBRARYDIR: %BOOST_LIBRARYDIR%
echo BOOST_ROOT: %BOOST_ROOT%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"