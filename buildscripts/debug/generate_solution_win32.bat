set SS_PLATFORM=win32
call "../details/prepare_environment.bat"
cd ..
cd ..
mkdir build
cd build
del CMakeCache.txt
set TESTS=enabled
cmake -G "Visual Studio 15 2017" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Debug
cd ..
cd ./buildscripts/debug/