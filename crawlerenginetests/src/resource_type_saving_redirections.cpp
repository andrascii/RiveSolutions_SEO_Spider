#include "storage_type.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(ResourceTypeSavingRedirections, SavingWithRedirectionToHtml)
{
	// In this test image redirectrs using 302 redirect to an html
	// all resource types must be saved

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://resource-type-saving-redirections.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> imagesResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		const std::vector<const ParsedPage*> htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);

		EXPECT_EQ(2, imagesResources.size());
		EXPECT_EQ(3, htmlResources.size());

		const std::vector<std::pair<Url, ResourceType>> urlTypes =
		{
			std::pair(Url("btclogo.png"), ResourceType::ResourceImage),
			std::pair(Url("not_existing_image.bmp"), ResourceType::ResourceImage),
			std::pair(Url("index.html"), ResourceType::ResourceHtml),
			std::pair(Url("404.html"), ResourceType::ResourceHtml),
			std::pair(Url("from404.html"), ResourceType::ResourceHtml)
		};

		for(const ParsedPage* parsedPage : cl->storageItems(StorageType::CrawledUrlStorageType))
		{
			const auto iter = std::find_if(urlTypes.begin(), urlTypes.end(), 
				[&parsedPage](const auto& p) { return p.first.toDisplayString() == parsedPage->url.fileName(); });

			const bool fileFound = iter != urlTypes.end();

			EXPECT_EQ(true, fileFound);

			if (!fileFound)
			{
				continue;
			}

			EXPECT_EQ(iter->second, parsedPage->resourceType);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

}