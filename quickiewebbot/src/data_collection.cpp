#include "data_collection.h"
#include "page_info.h"

namespace QuickieWebBot
{

DataCollection::DataCollection(QObject* parent)
	: QObject(parent)
	, m_crawlerStorageMap(std::initializer_list<std::pair<const int, CrawlerStorageTypePtr>>
	{
		std::make_pair(CrawledUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(ExternalUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(UpperCaseUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(NonAsciiCharacterUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(VeryLongUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(EmptyTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(DuplicatedTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(VeryLongTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(VeryShortTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(DuplicatedH1TitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(SeveralTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherTitle))))),

		std::make_pair(EmptyMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaDescription))))),

		std::make_pair(DuplicatedMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaDescription))))),

		std::make_pair(VeryLongMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaDescription))))),

		std::make_pair(VeryShortMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaDescription))))),

		std::make_pair(SeveralMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaDescription))))),

		std::make_pair(EmptyMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaKeywords))))),

		std::make_pair(DuplicatedMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaKeywords))))),

		std::make_pair(SeveralMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherMetaKeywords))))),

		std::make_pair(MissingH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH1))))),

		std::make_pair(DuplicatedH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH1))))),

		std::make_pair(VeryLongH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH1))))),

		std::make_pair(SeveralH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH1))))),

		std::make_pair(MissingH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH2))))),

		std::make_pair(DuplicatedH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH2))))),

		std::make_pair(VeryLongH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH2))))),

		std::make_pair(SeveralH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherFirstH2))))),

		std::make_pair(Over100kbImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(MissingAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl))))),

		std::make_pair(VeryLongAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, UniversalPageInfoHasher(
				std::shared_ptr<IPageInfoHasher>(new PageInfoHasherUrl)))))
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
{
}

bool DataCollection::isPageInfoExists(const PageInfoPtr& pageInfo, int storageType) const noexcept
{
	qDebug() << pageInfo->url.toString();

	checkStorageType(storageType);
	CrawlerStorageType const* pQueue = crawlerStorage(storageType);
	return pQueue->find(pageInfo) != pQueue->end();
}

void DataCollection::addPageInfo(const PageInfoPtr& pageInfo, int storageType) noexcept
{
	if (isPageInfoExists(pageInfo, storageType))
	{
		auto iter = crawlerStorage(storageType)->find(pageInfo);

		qDebug() << "not inserted because it the same as " << (*iter)->url;
		return;
	}

	crawlerStorage(storageType)->insert(pageInfo);
	guiStorage(storageType)->push_back(pageInfo);

	emit pageInfoAdded(static_cast<int>(guiStorage(storageType)->size() - 1), storageType);
}

DataCollection::GuiStorageType* DataCollection::guiStorage(int type) noexcept
{
	return m_guiStorageMap[type].get();
}

DataCollection::GuiStorageType const* DataCollection::guiStorage(int type) const noexcept
{
	const GuiStorageType* pQueue = const_cast<DataCollection* const>(this)->guiStorage(type);
	return pQueue;
}

DataCollection::CrawlerStorageType* DataCollection::crawlerStorage(int type) noexcept
{
	return m_crawlerStorageMap[type].get();
}

DataCollection::CrawlerStorageType const* DataCollection::crawlerStorage(int type) const noexcept
{
	const CrawlerStorageType* pQueue = const_cast<DataCollection* const>(this)->crawlerStorage(type);
	return pQueue;
}

void DataCollection::checkStorageType(int type) const noexcept
{
	assert(
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

}