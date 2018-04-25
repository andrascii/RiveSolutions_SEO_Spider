#include "application.h"

int main(int argc, char *argv[])
{
	using namespace SeoSpider;

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

	Application app(argc, argv);
	return app.exec();
}
