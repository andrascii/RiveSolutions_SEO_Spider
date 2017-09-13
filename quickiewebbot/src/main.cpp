#include "application.h"
#include "watch_dog_api.h"

int main(int argc, char *argv[])
{
	initWatchDogApi();

	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
