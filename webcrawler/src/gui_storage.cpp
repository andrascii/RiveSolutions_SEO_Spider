#include "gui_storage.h"

namespace WebCrawler
{

GuiStorage::GuiStorage()
{
}

GuiStorage::GuiStorageTypePtr& GuiStorage::storage(DataCollection::StorageType type) noexcept
{
	ASSERT(m_guiStorageMap.find(type) != m_guiStorageMap.end());
	return m_guiStorageMap.find(type)->second;
}

const GuiStorage::GuiStorageTypePtr& GuiStorage::storage(DataCollection::StorageType type) const noexcept
{
	const GuiStorageTypePtr& storage = const_cast<GuiStorage* const>(this)->storage(type);
	return storage;
}

void GuiStorage::addPageRaw(PageRawPtr pageRaw, int type) noexcept
{
	DataCollection::StorageType storageType = static_cast<DataCollection::StorageType>(type);
	auto guiStorageIt = m_guiStorageMap.find(storageType);

	if (guiStorageIt != m_guiStorageMap.end())
	{
		guiStorageIt->second->push_back(pageRaw);
		Q_EMIT pageRawAdded(storage(storageType)->size() - 1, storageType);
		//ASSERT(isPageRawExists(pageRaw, storageType));
	}
}

void GuiStorage::initializeStorages()
{
	using DC = DataCollection;

	m_guiStorageMap = std::initializer_list<std::pair<const int, GuiStorageTypePtr>>
	{
		std::make_pair(DC::CrawledUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::UpperCaseUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::NonAsciiCharacterUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::EmptyTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryShortTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedH1TitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::SeveralTitleUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::EmptyMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryShortMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::SeveralMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::EmptyMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::SeveralMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::MissingH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::SeveralH1UrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::MissingH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::DuplicatedH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::SeveralH2UrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::Over100kbImageStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::MissingAltTextImageStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VeryLongAltTextImageStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::Status404StorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::HtmlResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ImageResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::JavaScriptResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::StyleSheetResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::FlashResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::VideoResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::OtherResourcesStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(DC::ExternalHtmlResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalImageResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalJavaScriptResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalStyleSheetResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalFlashResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalVideoResourcesStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DC::ExternalOtherResourcesStorageType, std::make_shared<GuiStorageType>())
	};
}

}