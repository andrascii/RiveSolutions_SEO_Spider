#include "application.h"
#include "watch_dog_api.h"

int main(int argc, char *argv[])
{
	initializeWatchDog();

	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
