#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class ModelControllerData
{
private:
	using CrawlerQueueType = std::unordered_set<std::shared_ptr<WebsiteAnalyseElement>, WebsiteAnalyseElementHasher>;
	using GuiQueueType = std::vector<std::shared_ptr<WebsiteAnalyseElement>>;

public:
	enum QueueType
	{
		InternalQueueType,
		CrawledQueueType,
		ExternalQueueType
	};

	bool isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, QueueType type) const noexcept;
	void addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, QueueType type) noexcept;

private:
	void checkQueueType(QueueType type) const noexcept;

	CrawlerQueueType const* crawlerQueue(QueueType type) const noexcept;
	CrawlerQueueType* crawlerQueue(QueueType type) noexcept;

	GuiQueueType const* guiQueue(QueueType type) const noexcept;
	GuiQueueType* guiQueue(QueueType type) noexcept;

private:
	template <typename QueueType>
	struct Queues
	{
		// internal crawler url queues
		QueueType internalUrlsQueue;
		QueueType crawledUrlsQueue;
		QueueType externalUrlsQueue;

		// urls
		QueueType upperCaseUrlsQueue;
		QueueType nonAsciiCharacterUrlsQueue;
		QueueType veryLongUrlsQueue;

		// titles
		QueueType emptyTitleUrlsQueue;
		QueueType duplicatedTitleUrlsQueue;
		QueueType veryLongTitleUrlsQueue;
		QueueType veryShortTitleUrlsQueue;
		QueueType duplicatedH1TitleUrlsQueue;
		QueueType severalTitleUrlsQueue;

		// meta descriptions
		QueueType emptyMetaDescriptionUrlsQueue;
		QueueType duplicatedMetaDescriptionUrlsQueue;
		QueueType veryLongMetaDescriptionUrlsQueue;
		QueueType veryShortMetaDescriptionUrlsQueue;
		QueueType severalMetaDescriptionUrlsQueue;

		// meta keywords
		QueueType emptyMetaKeywordsUrlsQueue;
		QueueType duplicatedMetaKeywordsUrlsQueue;
		QueueType severalMetaKeywordsUrlsQueue;

		// H1
		QueueType missingH1UrlsQueue;
		QueueType duplicatedH1UrlsQueue;
		QueueType veryLongH1UrlsQueue;
		QueueType severalH1UrlsQueue;

		// H2
		QueueType missingH2UrlsQueue;
		QueueType duplicatedH2UrlsQueue;
		QueueType veryLongH2UrlsQueue;
		QueueType severalH2UrlsQueue;

		// images
		QueueType over100kbImagesQueue;
		QueueType missingAltTextImagesQueue;
		QueueType veryLongAltTextImagesQueue;
	};

private:
	Queues<CrawlerQueueType> m_crawlerQueues;
	Queues<GuiQueueType> m_guiQueues;
};

}
