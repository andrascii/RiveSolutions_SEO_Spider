#include "site_map.h"
#include "xpath_helpers.h"
#include "tests_downloader.h"
#include "download_response.h"

namespace CrawlerEngineTests
{

QString toRFC2822Date(const QDateTime& time)
{
	static QMap<int, QString> s_dayOfWeek
	{
		{ 1, "Mon" },
		{ 2, "Tue" },
		{ 3, "Wed" },
		{ 4, "Thu" },
		{ 5, "Fri" },
		{ 6, "Sat" },
		{ 7, "Sun" },
	};

	static QMap<int, QString> s_month
	{
		{ 1, "Jan" },
		{ 2, "Feb" },
		{ 3, "Mar" },
		{ 4, "Apr" },
		{ 5, "May" },
		{ 6, "Jun" },
		{ 7, "Jul" },
		{ 8, "Aug" },
		{ 9, "Sep" },
		{ 10, "Oct" },
		{ 11, "Nov" },
		{ 12, "Dec" }
	};

	QString result = s_dayOfWeek[time.date().dayOfWeek()];
	result += time.toString(", dd ");
	result += s_month[time.date().month()];
	result += time.toString(" yyyy hh:mm:ss") + QString(" GMT");

	DEBUG_ASSERT(QDateTime::fromString(result, Qt::RFC2822Date).isValid());

	return result;
}

TEST(SiteMapTests, SimpleSiteMap)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		CrawlerEngine::SiteMapSettings settings;
		const QString xml = cl->siteMapXml(settings);
		EXPECT_EQ(true, xml.contains(QString("page.html")));
		EXPECT_EQ(true, xml.contains(QString("page-2.html")));
		EXPECT_EQ(true, xml.contains(QString("page-3.html")));
		EXPECT_EQ(true, !xml.contains(QString("page-4.html"))); // discarded by noindex
		EXPECT_EQ(true, !xml.contains(QString("page-5.html"))); // discarded by status code
		EXPECT_EQ(true, !xml.contains(QString("btclogo.png"))); // images are discarded by default
		
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, LastModified)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeLastModTag);

		const QString xml = cl->siteMapXml(settings);
		const QString result = CrawlerEngine::XPathHelpers::evaluateXPath(xml, 
			QString("/urlset/url[1]/lastmod/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));

		
		EXPECT_EQ(QString("2017-11-07"), result);

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, FrequencyByHeader)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->testDownloader()->setPostProcessor([](CrawlerEngine::DownloadResponse& resp)
		{
			QDateTime time = QDateTime::currentDateTime().toUTC();
			const QString dateTIme = toRFC2822Date(time);
			resp.responseHeaders.addHeaderValue(QString("Last-Modified"), dateTIme);
		});

		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeChangeFreqTag);
		settings.changeFreqMode = CrawlerEngine::SiteMapChangeFreqTagMode::CalculatedFromLastModifiedHeader;

		const QString xml = cl->siteMapXml(settings);
		const QString result = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[1]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		EXPECT_EQ(QString("always"), result);

	};

	env.initializeTest(testFunction);
	env.exec();
}


}
