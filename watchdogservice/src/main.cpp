#include "watch_dog_service_app.h"

int main(int argc, char** argv)
{
	WatchDogService::WatchDogServiceApp app(argc, argv);

	return app.exec();
}