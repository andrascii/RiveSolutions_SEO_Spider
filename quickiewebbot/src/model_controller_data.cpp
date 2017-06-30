#include "model_controller_data.h"

namespace QuickieWebBot
{

bool ModelControllerData::isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, StorageType type) const noexcept
{
	checkStorageType(type);

	CrawlerStorageType const* pQueue = crawlerStorage(type);

	return pQueue->find(websiteAnalysElement) != std::end(*pQueue);
}

void ModelControllerData::addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, StorageType type) noexcept
{
	if (isElementExists(websiteAnalysElement, type))
	{
		return;
	}

	crawlerStorage(type)->insert(websiteAnalysElement);
	guiStorage(type)->push_back(websiteAnalysElement);
}

void ModelControllerData::checkStorageType(StorageType type) const noexcept
{
	assert(
		type == InternalUrlStorageType || 
		type == CrawledUrlStorageType || 
		type == ExternalUrlStorageType ||
		type == UpperCaseUrlStorageType ||
		type == NonAsciiCharacterUrlStorageType ||
		type == VeryLongUrlStorageType ||

		type == EmptyTitleUrlStorageType ||
		type == DuplicatedTitleUrlStorageType ||
		type == VeryLongTitleUrlStorageType ||
		type == VeryShortTitleUrlStorageType ||
		type == DuplicatedH1TitleUrlStorageType ||
		type == SeveralTitleUrlStorageType ||

		type == EmptyMetaDescriptionUrlStorageType ||
		type == DuplicatedMetaDescriptionUrlStorageType ||
		type == VeryLongMetaDescriptionUrlStorageType ||
		type == VeryShortMetaDescriptionUrlStorageType ||
		type == SeveralMetaDescriptionUrlStorageType ||

		type == EmptyMetaKeywordsUrlStorageType ||
		type == DuplicatedMetaKeywordsUrlStorageType ||
		type == SeveralMetaKeywordsUrlStorageType ||

		type == MissingH1UrlStorageType ||
		type == DuplicatedH1UrlStorageType ||
		type == VeryLongH1UrlStorageType ||
		type == SeveralH1UrlStorageType ||

		type == MissingH2UrlStorageType ||
		type == DuplicatedH2UrlStorageType ||
		type == VeryLongH2UrlStorageType ||
		type == SeveralH2UrlStorageType ||

		type == Over100kbImageStorageType ||
		type == MissingAltTextImageStorageType ||
		type == VeryLongAltTextImageStorageType
	);
}

ModelControllerData::CrawlerStorageType const* ModelControllerData::crawlerStorage(StorageType type) const noexcept
{
	CrawlerStorageType const* pQueue = nullptr;

	switch (type)
	{
		case InternalUrlStorageType:
		{
			pQueue = &m_crawlerQueues.internalUrlStorage;
			break;
		}

		case CrawledUrlStorageType:
		{
			pQueue = &m_crawlerQueues.crawledUrlStorage;
			break;
		}

		case ExternalUrlStorageType:
		{
			pQueue = &m_crawlerQueues.externalUrlStorage;

			break;
		}
	}

	return pQueue;
}

ModelControllerData::CrawlerStorageType* ModelControllerData::crawlerStorage(StorageType type) noexcept
{
	CrawlerStorageType const* pQueue = const_cast<ModelControllerData const * const>(this)->crawlerStorage(type);
	return const_cast<CrawlerStorageType*>(pQueue);
}

ModelControllerData::GuiStorageType const* ModelControllerData::guiStorage(StorageType type) const noexcept
{
	GuiStorageType const* pQueue = nullptr;

	switch (type)
	{
		case InternalUrlStorageType:
		{
			pQueue = &m_guiQueues.internalUrlStorage;
			break;
		}

		case CrawledUrlStorageType:
		{
			pQueue = &m_guiQueues.crawledUrlStorage;
			break;
		}

		case ExternalUrlStorageType:
		{
			pQueue = &m_guiQueues.externalUrlStorage;

			break;
		}
	}

	return pQueue;
}

ModelControllerData::GuiStorageType* ModelControllerData::guiStorage(StorageType type) noexcept
{
	GuiStorageType const* pQueue = const_cast<ModelControllerData const * const>(this)->guiStorage(type);
	return const_cast<GuiStorageType*>(pQueue);
}

}