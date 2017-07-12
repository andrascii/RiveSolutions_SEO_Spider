#include "application.h"
#include <Windows.h>

int main(int argc, char *argv[])
{
	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
