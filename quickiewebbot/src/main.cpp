#include "application.h"
#include "dll_loader.h"

int main(int argc, char *argv[])
{
	QuickieWebBot::Application app(argc, argv);

	return app.exec(); 
}
