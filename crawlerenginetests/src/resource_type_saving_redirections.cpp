#include "stdafx.h"
#include "storage_type.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(ResourceTypeSavingRedirections, SavingWith302RedirectionToHtml)
{
	// In this test image redirectrs using 302 redirect to an html
	// all resource types must be saved

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://resource-type-saving-redirections1.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> imagesResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		const std::vector<const ParsedPage*> htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);

		EXPECT_EQ(2, imagesResources.size());
		EXPECT_EQ(3, htmlResources.size());

		const std::vector<std::pair<Url, ResourceType>> urlTypes =
		{
			std::pair(Url("http://resource-type-saving-redirections1.com/btclogo.png"), ResourceType::ResourceImage),
			std::pair(Url("http://resource-type-saving-redirections1.com/not_existing_image.bmp"), ResourceType::ResourceImage),
			std::pair(Url("http://resource-type-saving-redirections1.com/index.html"), ResourceType::ResourceHtml),
			std::pair(Url("http://resource-type-saving-redirections1.com/404.html"), ResourceType::ResourceHtml),
			std::pair(Url("http://resource-type-saving-redirections1.com/from404.html"), ResourceType::ResourceHtml)
		};

		for(const ParsedPage* parsedPage : cl->storageItems(StorageType::CrawledUrlStorageType))
		{
			const auto iter = std::find_if(urlTypes.begin(), urlTypes.end(), 
				[&parsedPage](const auto& p) { return p.first.compare(parsedPage->url); });

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

TEST(ResourceTypeSavingRedirections, SavingWith301RedirectionToHtml)
{
	// In this test image redirectrs using 301 redirect to an html
	// all resource types must be saved

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://resource-type-saving-redirections2.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> imagesResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		const std::vector<const ParsedPage*> htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);

		EXPECT_EQ(2, imagesResources.size());
		EXPECT_EQ(3, htmlResources.size());

		const std::vector<std::pair<Url, ResourceType>> urlTypes =
		{
			std::pair(Url("http://resource-type-saving-redirections2.com/btclogo.png"), ResourceType::ResourceImage),
			std::pair(Url("http://resource-type-saving-redirections2.com/not_existing_image.bmp"), ResourceType::ResourceImage),
			std::pair(Url("http://resource-type-saving-redirections2.com/index.html"), ResourceType::ResourceHtml),
			std::pair(Url("http://resource-type-saving-redirections2.com/404.html"), ResourceType::ResourceHtml),
			std::pair(Url("http://resource-type-saving-redirections2.com/from404.html"), ResourceType::ResourceHtml)
		};

		for (const ParsedPage* parsedPage : cl->storageItems(StorageType::CrawledUrlStorageType))
		{
			const auto iter = std::find_if(urlTypes.begin(), urlTypes.end(),
				[&parsedPage](const auto& p) { return p.first.compare(parsedPage->url); });

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

TEST(ResourceTypeSavingRedirections, Image404)
{
	// In this test when loading image we get the 404 status code.
	// For this image condition "resourceType == ResourceImage" must be met

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://resource-type-saving-redirections3.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> imagesResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		const std::vector<const ParsedPage*> htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);

		EXPECT_EQ(1, imagesResources.size());
		EXPECT_EQ(1, htmlResources.size());

		const Url brokenImage("http://resource-type-saving-redirections3.com/not_existing_image.bmp");

		const std::vector<std::pair<Url, ResourceType>> urlTypes =
		{
			std::pair(brokenImage, ResourceType::ResourceImage),
			std::pair(Url("http://resource-type-saving-redirections3.com/index.html"), ResourceType::ResourceHtml)
		};

		for (const ParsedPage* parsedPage : cl->storageItems(StorageType::CrawledUrlStorageType))
		{
			const auto iter = std::find_if(urlTypes.begin(), urlTypes.end(),
				[&parsedPage](const auto& p) { return p.first.compare(parsedPage->url); });

			const bool fileFound = iter != urlTypes.end();

			EXPECT_EQ(true, fileFound);

			if (!fileFound)
			{
				continue;
			}

			EXPECT_EQ(iter->second, parsedPage->resourceType);
		}

		const std::vector<const ParsedPage*> brokenImages = cl->storageItems(StorageType::BrokenImagesStorageType);
		const std::vector<const ParsedPage*> brokenLinks = cl->storageItems(StorageType::Status4xxStorageType);

		EXPECT_EQ(1, brokenImages.size());
		EXPECT_EQ(1, brokenLinks.size());

		if (!brokenImages.empty())
		{
			EXPECT_EQ(brokenImage, brokenImages[0]->url);
		}

		if (!brokenLinks.empty())
		{
			EXPECT_EQ(brokenImage, brokenLinks[0]->url);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}


}