#include "site_map.h"
#include "xpath_helpers.h"
#include "tests_downloader.h"
#include "download_response.h"

namespace CrawlerEngineTests
{

QString toRFC2822Date(const QDateTime& time)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

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
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		const QString xml = cl->siteMapXml(settings);
		EXPECT_EQ(true, xml.contains(QString("page-1.html")));
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
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

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
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->testDownloader()->setPostProcessor([](CrawlerEngine::DownloadResponse& resp)
		{
			QDateTime time = QDateTime::currentDateTime().toUTC();
			//QString path = resp.url.path();
			if (resp.url.path() == QString("/page-2.html"))
			{
				time = time.addMSecs(-1000 * 60 * 15 - 1); // -15 minutes 
			}
			if (resp.url.path() == QString("/page-3.html"))
			{
				time = time.addMSecs(-1000 * 60 * 60 * 12 - 1); // -12 hours
			}

			const QString dateTIme = toRFC2822Date(time);
			resp.responseHeaders.removeHeaderValues(QString("Last-Modified"));
			resp.responseHeaders.addHeaderValue(QString("Last-Modified"), dateTIme);
		});

		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeChangeFreqTag);
		settings.changeFreqMode = CrawlerEngine::SiteMapChangeFreqTagMode::CalculatedFromLastModifiedHeader;

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[1]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result2 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[2]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result3 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[3]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		EXPECT_EQ(QString("always"), result1);
		EXPECT_EQ(QString("hourly"), result2);
		EXPECT_EQ(QString("daily"), result3);

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, FrequencyByLevel)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->testDownloader()->setPostProcessor([](CrawlerEngine::DownloadResponse& resp)
		{
			resp.statusCode = static_cast<int>(Common::StatusCode::Ok200);
		});

		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 8, 10, "Waiting for 8 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeChangeFreqTag);
		settings.flags.setFlag(CrawlerEngine::IncludeNoIndexPages);
		settings.changeFreqMode = CrawlerEngine::SiteMapChangeFreqTagMode::UseLevelSettings;
		
		settings.changeFreqLevelSettings[0] = CrawlerEngine::SitemapChangeFreq::Always;
		settings.changeFreqLevelSettings[1] = CrawlerEngine::SitemapChangeFreq::Hourly;
		settings.changeFreqLevelSettings[2] = CrawlerEngine::SitemapChangeFreq::Daily;
		settings.changeFreqLevelSettings[3] = CrawlerEngine::SitemapChangeFreq::Weekly;
		settings.changeFreqLevelSettings[4] = CrawlerEngine::SitemapChangeFreq::Monthly;
		settings.changeFreqLevelSettings[5] = CrawlerEngine::SitemapChangeFreq::Yearly;

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[1]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result2 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[2]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result3 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[3]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result4 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[4]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result5 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[5]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result6 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[6]/changefreq/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		EXPECT_EQ(QString("always"), result1); // level 1
		EXPECT_EQ(QString("always"), result2); // level 1
		EXPECT_EQ(QString("hourly"), result3); // level 2
		EXPECT_EQ(QString("daily"), result4); // level 3
		EXPECT_EQ(QString("weekly"), result5); // level 4
		EXPECT_EQ(QString("monthly"), result6); // level 5

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, PriorityTag)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->testDownloader()->setPostProcessor([](CrawlerEngine::DownloadResponse& resp)
		{
			resp.statusCode = static_cast<int>(Common::StatusCode::Ok200);
		});

		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 8, 10, "Waiting for 8 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeChangeFreqTag);
		settings.flags.setFlag(CrawlerEngine::IncludeNoIndexPages);
		settings.flags.setFlag(CrawlerEngine::IncludePriorityTag);

		settings.priorityLevelSettings[0] = 0.9;
		settings.priorityLevelSettings[1] = 0.8;
		settings.priorityLevelSettings[2] = 0.7;
		settings.priorityLevelSettings[3] = 0.6;
		settings.priorityLevelSettings[4] = 0.5;
		settings.priorityLevelSettings[5] = 0.4;

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[1]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result2 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[2]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result3 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[3]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result4 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[4]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result5 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[5]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));
		const QString result6 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url[6]/priority/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		EXPECT_EQ(QString("0.9"), result1); // level 1
		EXPECT_EQ(QString("0.9"), result2); // level 1
		EXPECT_EQ(QString("0.8"), result3); // level 2
		EXPECT_EQ(QString("0.7"), result4); // level 3
		EXPECT_EQ(QString("0.6"), result5); // level 4
		EXPECT_EQ(QString("0.5"), result6); // level 5

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DiscardCanonical)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/canonical/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		// index-canonical.html is discarded, but index-1-canonical.html is not cause there is a link <a> to it
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index.html")));
		EXPECT_EQ(false, result1.contains(QString("http://sitemap.com/canonical/index-canonical.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index-1.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index-1-canonical.html")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DontDiscardCanonical)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/canonical/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeCanonicalised);

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		// index-canonical.html is not discarded because we use CrawlerEngine::IncludeCanonicalised
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index-canonical.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index-1.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/canonical/index-1-canonical.html")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DiscardNextPrev)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/nextprev/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));


		// index-next.html/index-prev.html are discarded, but index-1-next.html/index-1-next.html are not cause there are the links <a> to them
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index.html")));
		EXPECT_EQ(false, result1.contains(QString("http://sitemap.com/nextprev/index-next.html")));
		EXPECT_EQ(false, result1.contains(QString("http://sitemap.com/nextprev/index-prev.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1-next.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1-prev.html")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DoNotDiscardNextPrev)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/nextprev/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludePaginatedUrls);

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));

		// index-next.html/index-prev.html are not discarded because we use CrawlerEngine::IncludePaginatedUrls
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-next.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-prev.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1-next.html")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/nextprev/index-1-prev.html")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DiscardNoImageIndex)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/images/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 7, 10, "Waiting for 7 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeImages);

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));

		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo.png")));
		EXPECT_EQ(false, result1.contains(QString("http://sitemap.com/images/btclogo-1.png")));
		EXPECT_EQ(false, result1.contains(QString("http://sitemap.com/images/btclogo-2.png")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo-3.png")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SiteMapTests, DoNotDiscardNoImageIndex)
{
	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/images/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 7, 10, "Waiting for 7 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		CrawlerEngine::SiteMapSettings settings;
		settings.flags.setFlag(CrawlerEngine::IncludeImages);
		settings.flags.setFlag(CrawlerEngine::IncludeNoIndexImages);

		const QString xml = cl->siteMapXml(settings);
		const QString result1 = CrawlerEngine::XPathHelpers::evaluateXPath(xml,
			QString("/urlset/url/loc/text()"), QString("http://www.sitemaps.org/schemas/sitemap/0.9"));

		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo.png")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo-1.png")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo-2.png")));
		EXPECT_EQ(true, result1.contains(QString("http://sitemap.com/images/btclogo-3.png")));
	};

	env.initializeTest(testFunction);
	env.exec();
}

}
