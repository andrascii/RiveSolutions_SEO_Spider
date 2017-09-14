#include "application.h"
#include "watch_dog_api.h"

void stackOverflowFunction(int var)
{
	stackOverflowFunction(var);
}

int main(int argc, char *argv[])
{
	initWatchDogApi();

	QuickieWebBot::Application app(argc, argv);
	stackOverflowFunction(42);
	return app.exec(); 
}
