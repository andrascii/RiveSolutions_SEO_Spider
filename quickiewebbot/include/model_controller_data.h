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
	struct CrawlerQueues
	{
		CrawlerQueueType internalUrlsQueue;
		CrawlerQueueType crawledUrlsQueue;
		CrawlerQueueType externalUrlsQueue;
	};

	struct GuiQueues
	{
		GuiQueueType internalUrlsQueue;
		GuiQueueType crawledUrlsQueue;
		GuiQueueType externalUrlsQueue;
	};

	CrawlerQueues m_crawlerQueues;
	GuiQueues m_guiQueues;
};

}
