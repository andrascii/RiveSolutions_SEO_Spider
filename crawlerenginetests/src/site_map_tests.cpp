#include "site_map.h"
#include "xpath_helpers.h"

namespace CrawlerEngineTests
{

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


}
