call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
D:
cd /
mkdir deps
cd deps
cd boost
call bootstrap
b2 -j8 toolset=msvc address-model=64 threading=multi runtime-link=shared --build-type=complete --stagedir=./x64 --build-dir=..\boost_build
b2 --clean-all -n
rmdir ..\boost_build /s /q 