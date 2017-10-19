
#include "tests_robots_txt_loader.h"

namespace WebCrawlerTests
{

void TestsRobotsTxtLoader::load(const QUrl& url)
{
	Q_UNUSED(url);
	emit ready(QByteArray());
}

QObject* TestsRobotsTxtLoader::qObject()
{
	return this;
}

}

