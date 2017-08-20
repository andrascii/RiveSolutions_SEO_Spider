#include "data_collection.h"
//#include "storage_adaptor.h"
//#include "storage_adaptor_factory.h"

namespace WebCrawler
{

DataCollection::DataCollection(QObject* parent)
	: QObject(parent)
	, m_crawlerStorageMap(std::initializer_list<std::pair<const int, CrawlerStorageTypePtr>>
	{
		std::make_pair(CrawledUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(ExternalUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(UpperCaseUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(NonAsciiCharacterUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(AllTitlesUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherTitle))))),

		std::make_pair(EmptyTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherTitle))))),

		std::make_pair(DuplicatedTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherTitle))))),

		std::make_pair(VeryShortTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherTitle))))),

		std::make_pair(DuplicatedH1TitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(SeveralTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherTitle))))),

		std::make_pair(AllMetaDescriptionsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaDescription))))),

		std::make_pair(EmptyMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaDescription))))),

		std::make_pair(DuplicatedMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaDescription))))),

		std::make_pair(VeryShortMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaDescription))))),

		std::make_pair(SeveralMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaDescription))))),

		std::make_pair(AllMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaKeywords))))),

		std::make_pair(EmptyMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaKeywords))))),

		std::make_pair(DuplicatedMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(SeveralMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherMetaKeywords))))),

		std::make_pair(AllH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH1))))),

		std::make_pair(MissingH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH1))))),

		std::make_pair(DuplicatedH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH1))))),

		std::make_pair(SeveralH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH1))))),

		std::make_pair(AllH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH2))))),

		std::make_pair(MissingH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH2))))),

		std::make_pair(DuplicatedH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH2))))),

		std::make_pair(SeveralH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherFirstH2))))),

		std::make_pair(Over100kbImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(MissingAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl))))),

		std::make_pair(VeryLongAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageRawHasher(
				std::shared_ptr<IPageRawHasher>(new PageRawHasherUrl)))))
	})
	, m_guiStorageMap(std::initializer_list<std::pair<const int, GuiStorageTypePtr>>
	{
		std::make_pair(CrawledUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(ExternalUrlStorageType, std::make_shared<GuiStorageType>()),
		
		std::make_pair(UpperCaseUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(NonAsciiCharacterUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongUrlStorageType, std::make_shared<GuiStorageType>()),
		
		std::make_pair(EmptyTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryShortTitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedH1TitleUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(SeveralTitleUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(EmptyMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryShortMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(SeveralMetaDescriptionUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(EmptyMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(SeveralMetaKeywordsUrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(MissingH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongH1UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(SeveralH1UrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(MissingH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(DuplicatedH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongH2UrlStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(SeveralH2UrlStorageType, std::make_shared<GuiStorageType>()),

		std::make_pair(Over100kbImageStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(MissingAltTextImageStorageType, std::make_shared<GuiStorageType>()),
		std::make_pair(VeryLongAltTextImageStorageType, std::make_shared<GuiStorageType>())
	})
	//, m_storageAdaptorFactory(std::make_unique<StorageAdaptorFactory>(this))
{
}

// StorageAdaptorFactory* DataCollection::storageAdaptorFactory()
// {
// 	return m_storageAdaptorFactory.get();
// }

bool DataCollection::isPageRawExists(const PageRawPtr& pageRaw, StorageType type) const noexcept
{
	checkStorageType(type);
	const CrawlerStorageTypePtr& storage = crawlerStorage(type);
	return storage->find(pageRaw) != storage->end();
}

const PageRawPtr& DataCollection::pageRaw(const PageRawPtr& pageRaw, StorageType type) const noexcept
{
	checkStorageType(type);
	assert(isPageRawExists(pageRaw, type));
	auto iter = crawlerStorage(type)->find(pageRaw);
	return *iter;
}

void DataCollection::addPageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept
{
	if (isPageRawExists(pageRaw, type))
	{
		assert(crawlerStorage(type)->size() > 0);
		auto iter = crawlerStorage(type)->find(pageRaw);
		return;
	}

	crawlerStorage(type)->insert(pageRaw);

	auto guiStorageIt = m_guiStorageMap.find(type);
	if (guiStorageIt != m_guiStorageMap.end())
	{
		guiStorageIt->second->push_back(pageRaw);
		Q_EMIT pageRawAdded(guiStorage(type)->size() - 1, type);
		assert(isPageRawExists(pageRaw, type));
	}	
}

DataCollection::GuiStorageTypePtr& DataCollection::guiStorage(StorageType type) noexcept
{
	assert(m_guiStorageMap.find(type) != m_guiStorageMap.end());
	return m_guiStorageMap[type];
}

const DataCollection::GuiStorageTypePtr& DataCollection::guiStorage(StorageType type) const noexcept
{
	const GuiStorageTypePtr& storage = const_cast<DataCollection* const>(this)->guiStorage(type);
	return storage;
}

DataCollection::CrawlerStorageTypePtr& DataCollection::crawlerStorage(StorageType type) noexcept
{
	return m_crawlerStorageMap[type];
}

const DataCollection::CrawlerStorageTypePtr& DataCollection::crawlerStorage(StorageType type) const noexcept
{
	const CrawlerStorageTypePtr& storage = const_cast<DataCollection* const>(this)->crawlerStorage(type);
	return storage;
}

void DataCollection::checkStorageType(StorageType type) const noexcept
{
	assert(
		type == CrawledUrlStorageType || 
		type == ExternalUrlStorageType ||
		type == UpperCaseUrlStorageType ||
		type == NonAsciiCharacterUrlStorageType ||
		type == VeryLongUrlStorageType ||

		type == AllTitlesUrlStorageType ||
		type == EmptyTitleUrlStorageType ||
		type == DuplicatedTitleUrlStorageType ||
		type == VeryLongTitleUrlStorageType ||
		type == VeryShortTitleUrlStorageType ||
		type == DuplicatedH1TitleUrlStorageType ||
		type == SeveralTitleUrlStorageType ||

		type == AllMetaDescriptionsUrlStorageType ||
		type == EmptyMetaDescriptionUrlStorageType ||
		type == DuplicatedMetaDescriptionUrlStorageType ||
		type == VeryLongMetaDescriptionUrlStorageType ||
		type == VeryShortMetaDescriptionUrlStorageType ||
		type == SeveralMetaDescriptionUrlStorageType ||

		type == AllMetaKeywordsUrlStorageType ||
		type == EmptyMetaKeywordsUrlStorageType ||
		type == DuplicatedMetaKeywordsUrlStorageType ||
		type == SeveralMetaKeywordsUrlStorageType ||

		type == AllH1UrlStorageType ||
		type == MissingH1UrlStorageType ||
		type == DuplicatedH1UrlStorageType ||
		type == VeryLongH1UrlStorageType ||
		type == SeveralH1UrlStorageType ||

		type == AllH2UrlStorageType ||
		type == MissingH2UrlStorageType ||
		type == DuplicatedH2UrlStorageType ||
		type == VeryLongH2UrlStorageType ||
		type == SeveralH2UrlStorageType ||

		type == Over100kbImageStorageType ||
		type == MissingAltTextImageStorageType ||
		type == VeryLongAltTextImageStorageType
	);
}

}