set SS_PLATFORM=x64
cd ..
cd ..
mkdir build
cd build
del CMakeCache.txt
set TESTS=enabled
cmake -G "Visual Studio 16 2019" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Release
cd ..
cd ./buildscripts/release/