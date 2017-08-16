#include "application.h"
#include "page_info.h"

int main(int argc, char *argv[])
{
	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
