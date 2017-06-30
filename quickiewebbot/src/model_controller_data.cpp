#include "model_controller_data.h"

namespace QuickieWebBot
{

bool ModelControllerData::isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, QueueType type) const noexcept
{
	checkQueueType(type);

	CrawlerQueueType const* pQueue = crawlerQueue(type);

	return pQueue->find(websiteAnalysElement) != std::end(*pQueue);
}

void ModelControllerData::addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, QueueType type) noexcept
{
	if (isElementExists(websiteAnalysElement, type))
	{
		return;
	}

	CrawlerQueueType* pQueue = crawlerQueue(type);

	pQueue->insert(websiteAnalysElement);

	//
	// TODO: add element to all queues
	//
}

void ModelControllerData::checkQueueType(QueueType type) const noexcept
{
	assert(type == InternalQueueType || 
		type == CrawledQueueType || 
		type == ExternalQueueType
	);
}

ModelControllerData::CrawlerQueueType const* ModelControllerData::crawlerQueue(QueueType type) const noexcept
{
	CrawlerQueueType const* pQueue = nullptr;

	switch (type)
	{
		case InternalQueueType:
		{
			pQueue = &m_crawlerQueues.internalUrlsQueue;
			break;
		}

		case CrawledQueueType:
		{
			pQueue = &m_crawlerQueues.crawledUrlsQueue;
			break;
		}

		case ExternalQueueType:
		{
			pQueue = &m_crawlerQueues.externalUrlsQueue;

			break;
		}
	}

	return pQueue;
}

ModelControllerData::CrawlerQueueType* ModelControllerData::crawlerQueue(QueueType type) noexcept
{
	CrawlerQueueType const* pQueue = const_cast<ModelControllerData const * const>(this)->crawlerQueue(type);
	return const_cast<CrawlerQueueType*>(pQueue);
}

ModelControllerData::GuiQueueType const* ModelControllerData::guiQueue(QueueType type) const noexcept
{
	GuiQueueType const* pQueue = nullptr;

	switch (type)
	{
		case InternalQueueType:
		{
			pQueue = &m_guiQueues.internalUrlsQueue;
			break;
		}

		case CrawledQueueType:
		{
			pQueue = &m_guiQueues.crawledUrlsQueue;
			break;
		}

		case ExternalQueueType:
		{
			pQueue = &m_guiQueues.externalUrlsQueue;

			break;
		}
	}

	return pQueue;
}

ModelControllerData::GuiQueueType* ModelControllerData::guiQueue(QueueType type) noexcept
{
	GuiQueueType const* pQueue = const_cast<ModelControllerData const * const>(this)->guiQueue(type);
	return const_cast<GuiQueueType*>(pQueue);
}

}