#include "application.h"
#include "page_raw_info.h"

int main(int argc, char *argv[])
{
	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
