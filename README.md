# QuickieWebBot
# Installing

All CMakeLists.txt files intended for creating project for Visual Studio.

Steps for build:

1. For this we need to get git-bash. (You can download it here: https://git-scm.com/downloads)
2. Create folder on your computer then open it and right-click, start Git Bash.
3. Type in "git clone https://bitbucket.org/andrey_ascii/quickiewebbot".

After that you have all sources. For now we need to generate Visual Studio project.
For that you need CMake. You can download it here: https://cmake.org/download/

4. Start CMake GUI
5. Select the folder in which you cloned repository.
6. Select the folder where you want to save the Visual Studio project files.
7. Click "Configure" and select your version of Visual Studio.
8. Click "Generate".
9. Click "Open Project".

Note:

For successful build and generating Visual Studio project you need boost library at least 1.64.0 version.
And also you have to be sure that (In case of OS Windows, I don't know what with Linux) 
in the system environment variables list you have two variables:

1. BOOST_ROOT which should be to set with path to the boost library. For instance d:/develop/boost/.
2. BOOST_LIBRARYDIR which should be to set like this: $BOOST_ROOT/stage/lib

Second variable in common case should to specify to the directory with compiled boost libraries.

Also you need to set up CMAKE_PREFIX_PATH to the folder where you have installed Qt framework for msvc compiler.
For example:

CMAKE_PREFIX_PATH = D:\program-files\Qt\Qt5.7.0\5.7\msvc2015_64
