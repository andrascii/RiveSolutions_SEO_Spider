#include "application.h"

#if defined Q_OS_WIN

//
// May be these libs need to link using CMake...
//

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "windowsapp.lib")

#endif

int main(int argc, char *argv[])
{
	using namespace QuickieWebBot;

	Application app(argc, argv);
	return app.exec(); 
}
