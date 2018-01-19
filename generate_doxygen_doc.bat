@echo Generating doxygen documentation

@set OUTPUT_DIRECTORY=%CD%\doxydoc
@set DOXYGEN_EXE=doxygen.exe

start /B doxygen.exe %CD%\doxyfile.dxg OUTPUT_DIRECTORY = %OUTPUT_DIRECTORY%
@pause