set SS_PLATFORM=x64
cd ..
cd ..
mkdir build
cd build
del CMakeCache.txt
set TESTS=enabled
cmake -G "Visual Studio 15 2017 Win64" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Debug
cd ..
cd ./buildscripts/debug/