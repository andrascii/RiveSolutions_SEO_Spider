#include "stdafx.h"
namespace CrawlerEngineTests
{

TEST(LinksTests, LinkAlt)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://links.com/link-with-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 100, "Waiting for 2 crawled pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(0, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);

		QString expectedTitle = QString::fromWCharArray(L"This is a broken link è ÷åòî ïî ðóññêè");
		EXPECT_EQ(expectedTitle, pages[0]->linksOnThisPage[0].altOrTitle);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, StylesheetLink)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://links.com/link-stylesheet.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(5);
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		auto cssPages = cl->storageItems(StyleSheetResourcesStorageType);
		EXPECT_EQ(1, cssPages.size());
		EXPECT_EQ(QString("http://links.com/styles.css"), cssPages.at(0)->url.toDisplayString());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, CanonicalNextPrev)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://links.com/canonical-next-prev.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(4, pages.size());

		auto canonical = pages[0]->linksOnThisPage[0];
		auto next = pages[0]->linksOnThisPage[2];
		auto prev = pages[0]->linksOnThisPage[1];

		EXPECT_EQ(QString("http://links.com/next-prev-canonical.html"), canonical.resource.lock()->url.toDisplayString());
		EXPECT_EQ(QString("http://links.com/next.html"), next.resource.lock()->url.toDisplayString());
		EXPECT_EQ(QString("http://links.com/prev.html"), prev.resource.lock()->url.toDisplayString());

		EXPECT_EQ(ResourceSource::SourceTagLinkRelCanonical, canonical.resourceSource);
		EXPECT_EQ(ResourceSource::SourceTagLinkRelNext, next.resourceSource);
		EXPECT_EQ(ResourceSource::SourceTagLinkRelPrev, prev.resourceSource);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, HrefLang)
{
	TestEnvironment env;
	auto options = TestEnvironment::defaultOptions(Url("http://links.com/hreflang.html"));
	options.crawlMetaHrefLangLinks = true;
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(3, pages.size());

		auto hreflang1 = pages[0]->linksOnThisPage[0];
		auto hreflang2 = pages[0]->linksOnThisPage[1];

		EXPECT_EQ(QString("http://links.com/hreflang-ru.html"), hreflang1.resource.lock()->url.toDisplayString());
		EXPECT_EQ(QString("http://links.com/hreflang-en.html"), hreflang2.resource.lock()->url.toDisplayString());

		EXPECT_EQ(ResourceSource::SourceTagLinkAlternateHrefLang, hreflang1.resourceSource);
		EXPECT_EQ(ResourceSource::SourceTagLinkAlternateHrefLang, hreflang2.resourceSource);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, NORM_FORMHrefLang)
{
	TestEnvironment env;
	auto options = TestEnvironment::defaultOptions(Url("http://links.com/hreflang.html"));
	options.crawlMetaHrefLangLinks = false;
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(1, pages.size());

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, NofollowLinksMustNotBeLoaded)
{
	TestEnvironment env;

	CrawlerOptionsData options = TestEnvironment::defaultOptions(Url("http://nofollowlinks.com"));
	options.followInternalNofollow = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		std::vector<Url> nofollowLinks;

		const auto addNofollowLink = [&nofollowLinks](const auto& resourceLink)
		{
			if (resourceLink.resource.expired() || resourceLink.linkParameter != LinkParameter::NofollowParameter)
			{
				return;
			}

			nofollowLinks.push_back(resourceLink.resource.lock().get()->url);
		};

		const auto collectNofollowLinks = [&addNofollowLink](auto* page)
		{
			std::for_each(page->linksOnThisPage.begin(), page->linksOnThisPage.end(), addNofollowLink);
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), collectNofollowLinks);

		const auto checkLinks = [&nofollowLinks](auto* page)
		{
			for (const QUrl& nofollowLink : nofollowLinks)
			{
				EXPECT_NE(nofollowLink, page->url);
			}
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BaseUrl)
{
	TestEnvironment env;

	const Url baseUrl("http://links.com/baseurl/index.html");
	CrawlerOptionsData options = TestEnvironment::defaultOptions(baseUrl);

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler(), &baseUrl]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto firstPage = pages.at(0);
		auto linksOnThisPage = firstPage->linksOnThisPage;
		for (auto link : linksOnThisPage)
		{
			EXPECT_TRUE(!link.url.toDisplayString().contains("/baseurl"));
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, SubdomainsMustNotBeLoaded)
{
	TestEnvironment env;

	const Url baseUrl("http://subdomains.com");
	CrawlerOptionsData options = TestEnvironment::defaultOptions(baseUrl);

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler(), &baseUrl]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		std::vector<QUrl> subdomainLinks;

		const auto addSubdomainLink = [&subdomainLinks, &baseUrl](const auto& resourceLink)
		{
			if (!PageParserHelpers::isSubdomain(baseUrl, resourceLink.url))
			{
				return;
			}

			subdomainLinks.push_back(resourceLink.url);
		};

		const auto collectSubdomainLinks = [&addSubdomainLink](auto* page)
		{
			std::for_each(page->linksOnThisPage.begin(), page->linksOnThisPage.end(), addSubdomainLink);
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), collectSubdomainLinks);

		EXPECT_EQ(true, !subdomainLinks.empty());

		const auto checkLinks = [&subdomainLinks](auto* page)
		{
			for (const QUrl& subdomainLink : subdomainLinks)
			{
				EXPECT_NE(subdomainLink, page->url);
			}
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedByOutsideFolderLinksMustNotBeLoaded)
{
	TestEnvironment env;
	auto options = TestEnvironment::defaultOptions(Url("http://links.com/folder1/folder2/"));
	options.crawlOutsideOfStartFolder = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(pages.size(), 2);

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		const QUrl blockedByOutsideFolderUrl("http://links.com/folder1/index.html");

		const auto checkLinks = [&blockedByOutsideFolderUrl](auto* page)
		{
			EXPECT_NE(blockedByOutsideFolderUrl, page->url);
		};

		//std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedByRobotsTxtLinksMustNotBeLoaded)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://blockedbyrobotstxt.com"));
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		const QUrl blockedByRobotsUrl("http://blockedbyrobotstxt.com/blocked.html");

		const auto checkLinks = [&blockedByRobotsUrl](auto* page)
		{
			const bool valid = blockedByRobotsUrl != page->url || page->statusCode == Common::StatusCode::BlockedByRobotsTxt;
			EXPECT_EQ(true, valid);
		};

		std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, Canonical)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://links.com/canonical-other.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(Url("http://links.com/canonical-canonical.html"), pages[0]->canonicalUrl);

		auto uniqueCanonicalPages = cl->storageItems(StorageType::UniqueCanonicalUrlResourcesStorageType);
		EXPECT_EQ(1, uniqueCanonicalPages.size());

		auto pagesWithValidCanonical = cl->storageItems(StorageType::AllCanonicalUrlResourcesStorageType);
		EXPECT_EQ(2, pagesWithValidCanonical.size());

		auto duplicatedCanonical = cl->storageItems(StorageType::DuplicatedCanonicalUrlResourcesStorageType);
		EXPECT_EQ(2, duplicatedCanonical.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, ExternalDoFollow)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/external-do-follow.html"));
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(2, pages.size());

		auto externalDoFollowPages = cl->storageItems(StorageType::ExternalDoFollowUrlResourcesStorageType);
		EXPECT_EQ(1, externalDoFollowPages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, DiscardAllExternalLinks)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/external.html"));
	options.checkExternalLinks = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(1, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, DiscardOnlyNoFollowExternalLinks)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/external.html"));
	options.checkExternalLinks = true;
	options.followExternalNofollow = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(2, pages.size());

		auto externalDoFollowPages = cl->storageItems(StorageType::ExternalDoFollowUrlResourcesStorageType);
		EXPECT_EQ(QString("http://external.com/link1"), externalDoFollowPages.at(0)->url.toDisplayString());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedResourcesByMetaRobots)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/metarobots/index.html"));
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);

		EXPECT_EQ(1, pages.size());

		EXPECT_EQ(1, pages[0]->linksOnThisPage.size());

		const std::vector<const ParsedPage*> blockedPages = cl->getLinksFromUnorderedDataCollection(StorageType::BlockedForSEIndexingStorageType);

		EXPECT_EQ(false, pages[0]->linksOnThisPage.front().resource.expired());

		const ParsedPage* blockedResource = pages[0]->linksOnThisPage.front().resource.lock().get();

		const bool isResourceBlocked = std::find(blockedPages.begin(), blockedPages.end(), blockedResource) != blockedPages.end();

		EXPECT_EQ(true, isResourceBlocked);
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(LinksTests, SecondGetRequest)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/second-get-head/second-get.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(3, pages.size());
		EXPECT_EQ("Second Get 3", pages.at(2)->title);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, SecondHeadRequest)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://links.com/second-get-head/second-head.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(3, pages.size());
		EXPECT_EQ("Second Head 3", pages.at(1)->title);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedByRobotsTxtRedirect)
{
	// index.html redirected to index-redirected.html
	// btclogo.png redirected to btclogo-redirected.png

	// index-redirected.html is blocked by robots.txt, but should be loaded as it's a redirected url (??? maybe shouldn't?)
	// Disallow: /*redirected
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects.com/index.html"));
	options.parserTypeFlags = ImagesResourcesParserType;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(4, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto imagePages = cl->storageItems(ImageResourcesStorageType);

		EXPECT_EQ(2, htmlPages.size());
		EXPECT_EQ(2, imagePages.size());

		EXPECT_EQ(true, !htmlPages.at(1)->linksToThisPage.empty());
		EXPECT_EQ(false, htmlPages.at(1)->linksToThisPage.at(0).resource.expired());

		EXPECT_EQ(true, !imagePages.at(1)->linksToThisPage.empty());
		EXPECT_EQ(false, imagePages.at(1)->linksToThisPage.at(0).resource.expired());
	};

	env.initializeTest(testFunction);
	env.exec();
}

const ResourceLink* getLink(const std::deque<ResourceLink>& links, const QString& urlPart)
{
	auto it = std::find_if(links.begin(), links.end(), [&urlPart](const ResourceLink& link)
	{
		return link.url.toDisplayString().contains(urlPart);
	});

	if (it != links.end())
	{
		return &(*it);
	}

	return nullptr;
}

const ParsedPage* getPage(const std::vector<const ParsedPage*>& pages, const QString& urlPart)
{
	auto it = std::find_if(pages.begin(), pages.end(), [&urlPart](const ParsedPage* page)
	{
		return page->url.toDisplayString().contains(urlPart);
	});

	if (it != pages.end())
	{
		return *it;
	}

	return nullptr;
}

TEST(LinksTests, MergeInternalRedirects)
{
	// index.html redirected to index-redirected.html
	// btclogo.png redirected to btclogo-redirected.png
	// but merge.html has direct links to index-redirected.html and btclogo-redirected.png
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects.com/merge.html"));
	options.parserTypeFlags = ImagesResourcesParserType;
	options.followRobotsTxtRules = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(5, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto imagePages = cl->storageItems(ImageResourcesStorageType);

		EXPECT_EQ(3, htmlPages.size());
		EXPECT_EQ(2, imagePages.size());

		const std::deque<ResourceLink>& mergeHtmlPageLinks = htmlPages.at(0)->linksOnThisPage;
		EXPECT_EQ(3, mergeHtmlPageLinks.size());

		auto indexRedirectedLink = getLink(mergeHtmlPageLinks, "index-redirected.html");
		auto btcLogoRedirectedLink = getLink(mergeHtmlPageLinks, "btclogo-redirected.png");

		EXPECT_TRUE(indexRedirectedLink != nullptr);
		EXPECT_TRUE(btcLogoRedirectedLink != nullptr);

		if (indexRedirectedLink != nullptr)
		{
			EXPECT_TRUE(!indexRedirectedLink->resource.expired() && indexRedirectedLink->resource.lock()->linksToThisPage.size() == 2);
		}

		if (btcLogoRedirectedLink != nullptr)
		{
			EXPECT_TRUE(!btcLogoRedirectedLink->resource.expired() && btcLogoRedirectedLink->resource.lock()->linksToThisPage.size() == 2);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, SimpleJsRedirect)
{
	// script.js redirected to script-redirected.js
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects.com/testjs.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;
	options.followRobotsTxtRules = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(3, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto jsPages = cl->storageItems(JavaScriptResourcesStorageType);

		// script.js with 301 code is interpreted as a js resource because it was found in the <script> tag
		EXPECT_EQ(1, htmlPages.size());
		EXPECT_EQ(2, jsPages.size());

		if (pages.size() == 3)
		{
			auto scriptjsPage = pages[1];
			auto scriptjsredirectedPage = pages[1];

			EXPECT_TRUE(scriptjsPage->linksOnThisPage.size() == 1 && !scriptjsPage->linksOnThisPage[0].resource.expired());
			EXPECT_TRUE(scriptjsredirectedPage->linksToThisPage.size() == 1 && !scriptjsredirectedPage->linksToThisPage[0].resource.expired());
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, MergeJsRedirect)
{
	// script.js redirected to script-redirected.js
	// testjmerge.html has a direct link to script-redirected.js
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects.com/testjsmerge.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;
	options.followRobotsTxtRules = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(4, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto jsPages = cl->storageItems(JavaScriptResourcesStorageType);

		// script.js with 301 code is interpreted as a js resource because it was found in the <script> tag
		EXPECT_EQ(2, htmlPages.size());
		EXPECT_EQ(2, jsPages.size());

		if (pages.size() == 4)
		{
			const ParsedPage* scriptjsPage = getPage(pages, "script.js");
			const ParsedPage* scriptjsredirectedPage = getPage(pages, "script-redirected.js");

			EXPECT_TRUE(scriptjsPage != nullptr);
			EXPECT_TRUE(scriptjsredirectedPage != nullptr);

			if (scriptjsPage != nullptr)
			{
				EXPECT_EQ(ResourceType::ResourceJavaScript, scriptjsPage->resourceType);
				EXPECT_TRUE(scriptjsPage->linksOnThisPage.size() == 1 && !scriptjsPage->linksOnThisPage[0].resource.expired());
			}

			if (scriptjsredirectedPage != nullptr)
			{
				EXPECT_TRUE(scriptjsredirectedPage->linksToThisPage.size() == 2 &&
					!scriptjsredirectedPage->linksToThisPage[0].resource.expired() &&
					!scriptjsredirectedPage->linksToThisPage[1].resource.expired());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, MergeJsRedirectWithHtmlLinkToJs)
{
	// script.js redirected to script-redirected.js
	// testjmerge.html has a direct link to script-redirected.js
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects.com/testjsmergewithahreflink.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;
	options.followRobotsTxtRules = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(4, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto jsPages = cl->storageItems(JavaScriptResourcesStorageType);

		EXPECT_EQ(2, htmlPages.size());
		EXPECT_EQ(2, jsPages.size());

		if (pages.size() == 4)
		{
			const ParsedPage* scriptjsPage = getPage(pages, "script.js");
			const ParsedPage* scriptjsredirectedPage = getPage(pages, "script-redirected.js");

			EXPECT_TRUE(scriptjsPage != nullptr);
			EXPECT_TRUE(scriptjsredirectedPage != nullptr);

			if (scriptjsPage != nullptr)
			{
				EXPECT_EQ(ResourceType::ResourceJavaScript, scriptjsPage->resourceType);
				EXPECT_TRUE(scriptjsPage->linksOnThisPage.size() == 1 && !scriptjsPage->linksOnThisPage[0].resource.expired());
			}

			if (scriptjsredirectedPage != nullptr)
			{
				EXPECT_TRUE(scriptjsredirectedPage->linksToThisPage.size() == 2 &&
					!scriptjsredirectedPage->linksToThisPage[0].resource.expired() &&
					!scriptjsredirectedPage->linksToThisPage[1].resource.expired());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, ExternalHtmlRedirect)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects2.com/index.html"));
	options.followRobotsTxtRules = false;
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(3, pages.size());

		if (pages.size() == 3)
		{
			const ParsedPage* externalPageWithRedirect = pages[1];
			const ParsedPage* redirectedExternalPage = pages[2];

			EXPECT_TRUE(externalPageWithRedirect->linksOnThisPage.size() == 1 && !externalPageWithRedirect->linksOnThisPage[0].resource.expired());
			EXPECT_TRUE(redirectedExternalPage->linksToThisPage.size() == 1 && !redirectedExternalPage->linksToThisPage[0].resource.expired());
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, MergeExternalRedirect)
{
	// index.html redirected to index-redirected.html
	// btclogo.png redirected to btclogo-redirected.png
	// but merge.html has direct links to index-redirected.html and btclogo-redirected.png
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects2.com/merge.html"));
	options.parserTypeFlags = 0;
	options.followRobotsTxtRules = false;
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(3, pages.size());

		if (pages.size() == 3)
		{
			const ParsedPage* externalWithRedirect = getPage(pages, "redirects.com/index.html");
			const ParsedPage* redirectedExternal = getPage(pages, "redirects.com/index-redirected.html");

			EXPECT_TRUE(externalWithRedirect != nullptr);
			EXPECT_TRUE(redirectedExternal != nullptr);

			if (redirectedExternal != nullptr)
			{
				EXPECT_TRUE(redirectedExternal->linksToThisPage.size() == 2 &&
					!redirectedExternal->linksToThisPage[0].resource.expired() &&
					!redirectedExternal->linksToThisPage[1].resource.expired());
			}

			if (externalWithRedirect != nullptr)
			{
				EXPECT_TRUE(externalWithRedirect->linksOnThisPage.size() == 1 && !externalWithRedirect->linksOnThisPage[0].resource.expired());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, MergeExternalJsRedirect)
{
	// script.js redirected to script-redirected.js
	// testjmerge.html has a direct link to script-redirected.js
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects2.com/testjsmerge.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;
	options.followRobotsTxtRules = false;
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(4, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto jsPages = cl->storageItems(ExternalJavaScriptResourcesStorageType);

		// script.js with 301 code is interpreted as a js resource because it was found in the <script> tag
		EXPECT_EQ(2, htmlPages.size());
		EXPECT_EQ(2, jsPages.size());

		if (pages.size() == 4)
		{
			const ParsedPage* scriptjsPage = getPage(pages, "script.js");
			const ParsedPage* scriptjsredirectedPage = getPage(pages, "script-redirected.js");

			EXPECT_TRUE(scriptjsPage != nullptr);
			EXPECT_TRUE(scriptjsredirectedPage != nullptr);

			if (scriptjsPage != nullptr)
			{
				EXPECT_EQ(ResourceType::ResourceJavaScript, scriptjsPage->resourceType);
				EXPECT_TRUE(scriptjsPage->linksOnThisPage.size() == 1 && !scriptjsPage->linksOnThisPage[0].resource.expired());
			}

			if (scriptjsredirectedPage != nullptr)
			{
				EXPECT_TRUE(scriptjsredirectedPage->linksToThisPage.size() == 2 &&
					!scriptjsredirectedPage->linksToThisPage[0].resource.expired() &&
					!scriptjsredirectedPage->linksToThisPage[1].resource.expired());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, MergeExternalJsRedirectWithHtmlLinkToJs)
{
	// script.js redirected to script-redirected.js
	// testjmerge.html has a direct link to script-redirected.js
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://redirects2.com/testjsmergewithahreflink.html"));
	options.parserTypeFlags = JavaScriptResourcesParserType;
	options.followRobotsTxtRules = false;
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(100);
		EXPECT_EQ(4, pages.size());

		auto htmlPages = cl->storageItems(HtmlResourcesStorageType);
		auto jsPages = cl->storageItems(ExternalJavaScriptResourcesStorageType);

		EXPECT_EQ(2, htmlPages.size());
		EXPECT_EQ(2, jsPages.size());

		if (pages.size() == 4)
		{
			const ParsedPage* scriptjsPage = getPage(pages, "script.js");
			const ParsedPage* scriptjsredirectedPage = getPage(pages, "script-redirected.js");

			EXPECT_TRUE(scriptjsPage != nullptr);
			EXPECT_TRUE(scriptjsredirectedPage != nullptr);

			if (scriptjsPage != nullptr)
			{
				EXPECT_EQ(ResourceType::ResourceJavaScript, scriptjsPage->resourceType);
				EXPECT_TRUE(scriptjsPage->linksOnThisPage.size() == 1 && !scriptjsPage->linksOnThisPage[0].resource.expired());
			}

			if (scriptjsredirectedPage != nullptr)
			{
				EXPECT_TRUE(scriptjsredirectedPage->linksToThisPage.size() == 2 &&
					!scriptjsredirectedPage->linksToThisPage[0].resource.expired() &&
					!scriptjsredirectedPage->linksToThisPage[1].resource.expired());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, SkipLoadingNofollowPage)
{
	// Scan website.
	// Follow by all links except that is the nofollow one
	// Verify that all storages contains the valid data:
	// All crawled page count == 3 (because we skip nofollow page)

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://nofollowlinks.com/allnofollow/index.html"));
	options.followInternalNofollow = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(3, pages.size());

		auto nofollowPages = cl->storageItems(NofollowLinksStorageType);
		EXPECT_EQ(1, nofollowPages.size());

		EXPECT_EQ(3, nofollowPages.at(0)->linksToThisPage.size());

		auto blocked = cl->storageItems(BlockedForSEIndexingStorageType);
		EXPECT_EQ(1, blocked.size());

		auto crawledPages = cl->storageItems(CrawledUrlStorageType);
		auto crawledIt = std::find_if(crawledPages.begin(), crawledPages.end(), [](const CrawlerEngine::ParsedPage* page)
		{
			return page->url.toDisplayString() == QString("http://nofollowlinks.com/allnofollow/nofollow.html");
		});

		EXPECT_EQ(crawledPages.end(), crawledIt);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, LoadNofollowPage)
{
	// Scan website.
	// Follow by all links
	// Verify that all storages contains the valid data:
	// BlockedForSEIndexingStorageType and NofollowLinksStorageType must contain one page

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://nofollowlinks.com/allnofollow/index.html"));
	options.followInternalNofollow = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(4, pages.size());

		auto nofollowPages = cl->storageItems(NofollowLinksStorageType);
		EXPECT_EQ(1, nofollowPages.size());

		EXPECT_EQ(3, nofollowPages.at(0)->linksToThisPage.size());

		auto blocked = cl->storageItems(BlockedForSEIndexingStorageType);
		EXPECT_EQ(1, blocked.size());

		auto crawledPages = cl->storageItems(CrawledUrlStorageType);
		auto crawledIt = std::find_if(crawledPages.begin(), crawledPages.end(), [](const CrawlerEngine::ParsedPage* page)
		{
			return page->url.toDisplayString() == QString("http://nofollowlinks.com/allnofollow/nofollow.html");
		});

		EXPECT_NE(crawledPages.end(), crawledIt);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, PartialNofollow)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://nofollowlinks.com/partialnofollow/index.html"));
	options.followInternalNofollow = false;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(5, pages.size());

		auto nofollowPages = cl->storageItems(NofollowLinksStorageType);
		EXPECT_EQ(0, nofollowPages.size());

		auto blocked = cl->storageItems(BlockedForSEIndexingStorageType);
		EXPECT_EQ(0, blocked.size());

		auto it = std::find_if(pages.begin(), pages.end(), [](const CrawlerEngine::ParsedPage* page)
		{
			return page->title == QString("Partial nofollow");
		});

		EXPECT_NE(pages.end(), it);

		auto crawledPages = cl->storageItems(CrawledUrlStorageType);
		auto crawledIt = std::find_if(crawledPages.begin(), crawledPages.end(), [](const CrawlerEngine::ParsedPage* page)
		{
			return page->url.toDisplayString() == QString("http://nofollowlinks.com/partialnofollow/nofollow.html");
		});

		EXPECT_NE(crawledPages.end(), crawledIt);
		QString title = (*crawledIt)->title;
		EXPECT_EQ(QString("Partial nofollow"), title);

		// TODO: check another storages
	};

	env.initializeTest(testFunction);
	env.exec();
}

}