#include "application.h"
#include "custom_uri_channel.h"

int main(int argc, char *argv[])
{
	using namespace SeoSpider;

	if (argc > 1)
	{
		const char* arg = argv[1];
		const QByteArray argStr = QByteArray::fromRawData(arg, static_cast<int>(strlen(arg)));

#ifdef USE_CUSTOM_URI_CHANNEL
		if (argStr.startsWith("seospider:"))
		{
			if (CrawlerEngine::CustomUrlChannel::writeUri(argStr))
			{
				return 0;

			}
			// application is not started or something went wrong
			// TODO: start application with params
			return 1;
		}
#endif

	}

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

	Application app(argc, argv);
	return app.exec();
}
