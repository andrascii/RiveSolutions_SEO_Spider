
#include "tests_robots_txt_loader.h"

namespace CrawlerEngineTests
{

void TestsRobotsTxtLoader::setUserAgent(const QByteArray& userAgent)
{
	// do nothing
}

void TestsRobotsTxtLoader::load(const QUrl& url)
{
	Q_UNUSED(url);
	emit ready();
}

const QByteArray& TestsRobotsTxtLoader::content() const noexcept
{
	return m_content;
}

bool TestsRobotsTxtLoader::isReady() const noexcept
{
	return true;
}

QObject* TestsRobotsTxtLoader::qobject()
{
	return this;
}

}

