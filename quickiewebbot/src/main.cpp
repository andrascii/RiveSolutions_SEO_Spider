#include "application.h"
#include "dll_loader.h"

int main(int argc, char *argv[])
{
	using namespace QuickieWebBot;

	Application app(argc, argv);
	return app.exec(); 
}
