set SS_PLATFORM=win32
call "./details/prepare_environment.bat"
cd ..
mkdir build
cd build
del CMakeCache.txt
set TESTS=disabled
cmake -G "Visual Studio 15 2017" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Release -DENABLE_STATISTICS=true
msbuild.exe seospider.sln /t:Build /p:Configuration=Release;Platform=Win32
msbuild.exe .\seospiderinstaller\seospiderinstaller.vcxproj /t:Build /p:Configuration=Release;Platform=Win32
cd ..
cd ./buildscripts/