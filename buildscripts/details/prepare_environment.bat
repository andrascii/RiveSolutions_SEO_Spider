set SS_DEPS=D:\deps
set CMAKE_PREFIX_PATH=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%
set BOOST_LIBRARYDIR=%SS_DEPS%\boost\%SS_PLATFORM%\lib
set BOOST_ROOT=%SS_DEPS%\boost
set path=%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\bin;%SS_DEPS%\qt\msvc_2017_%SS_PLATFORM%\plugins\platforms;%SS_DEPS%\openssl\openssl_%SS_PLATFORM%\bin;%SS_DEPS%\icu\icu_%SS_PLATFORM%\lib;%path%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"