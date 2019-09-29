set SS_PLATFORM=x64
cd ..
mkdir build
cd build
del CMakeCache.txt
set TESTS=disabled
cmake -G "Visual Studio 15 2017 Win64" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Release -DENABLE_STATISTICS=true
msbuild.exe .\seospiderinstaller\seospiderinstaller.vcxproj /t:Build /p:Configuration=Release;Platform=x64
cd ..
cd ./buildscripts/