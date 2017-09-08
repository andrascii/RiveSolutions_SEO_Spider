#include "data_collection.h"
#include "page_raw_comparator.h"
#include "gui_storage.h"

namespace WebCrawler
{

DataCollection::DataCollection(QObject* parent)
	: QObject(parent)
	, m_guiStorage(new GuiStorage)
{
	initializeStorages();


	m_guiStorage->moveToThread(QApplication::instance()->thread());
	QObject::connect(this, &DataCollection::pageRawAdded, m_guiStorage, &GuiStorage::addPageRaw, Qt::QueuedConnection);
}

DataCollection::~DataCollection()
{
	m_guiStorage->deleteLater();
}

bool DataCollection::isPageRawExists(const PageRawPtr& pageRaw, StorageType type) const noexcept
{
	checkStorageType(type);

	const CrawlerStorageTypePtr& storage = crawlerStorage(type);
	return storage->find(pageRaw) != storage->end();
}

const PageRawPtr DataCollection::pageRaw(const PageRawPtr& pageRaw, StorageType type) const noexcept
{
	checkStorageType(type);

	const CrawlerStorageTypePtr& storage = crawlerStorage(type);
	auto iter = storage->find(pageRaw);

	return iter != storage->end() ? *iter : PageRawPtr();
}

WebCrawler::GuiStorage* DataCollection::guiStorage() const noexcept
{
	return m_guiStorage;
}

void DataCollection::addPageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept
{
	if (isPageRawExists(pageRaw, type))
	{
		ASSERT(crawlerStorage(type)->size() > 0);
		auto iter = crawlerStorage(type)->find(pageRaw);
		return;
	}

	crawlerStorage(type)->insert(pageRaw);
	emit pageRawAdded(pageRaw, type);
}

PageRawPtr DataCollection::removePageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept
{
	checkStorageType(type);

	const CrawlerStorageTypePtr& storage = crawlerStorage(type);
	auto iter = storage->find(pageRaw);

	if (iter != storage->end())
	{
		PageRawPtr result = *iter;
		storage->erase(iter);
		return result;
	}

	return PageRawPtr();
}

DataCollection::CrawlerStorageTypePtr& DataCollection::crawlerStorage(StorageType type) noexcept
{
	ASSERT(m_crawlerStorageMap.find(type) != m_crawlerStorageMap.end());
	return m_crawlerStorageMap.find(type)->second;
}

const DataCollection::CrawlerStorageTypePtr& DataCollection::crawlerStorage(StorageType type) const noexcept
{
	const CrawlerStorageTypePtr& storage = const_cast<DataCollection* const>(this)->crawlerStorage(type);
	return storage;
}

void DataCollection::checkStorageType(StorageType type) const noexcept
{
	ASSERT(
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
		type == VeryLongAltTextImageStorageType ||

		type == Status404StorageType ||

		type == PendingResourcesStorageType ||
		type == HtmlResourcesStorageType ||
		type == ImageResourcesStorageType ||
		type == JavaScriptResourcesStorageType ||
		type == StyleSheetResourcesStorageType ||
		type == FlashResourcesStorageType ||
		type == VideoResourcesStorageType ||
		type == OtherResourcesStorageType ||

		type == ExternalHtmlResourcesStorageType ||
		type == ExternalImageResourcesStorageType ||
		type == ExternalJavaScriptResourcesStorageType ||
		type == ExternalStyleSheetResourcesStorageType ||
		type == ExternalFlashResourcesStorageType ||
		type == ExternalVideoResourcesStorageType ||
		type == ExternalOtherResourcesStorageType
	);
}


void DataCollection::initializeStorages()
{
	m_crawlerStorageMap = std::initializer_list<std::pair<const int, CrawlerStorageTypePtr>>
	{
		std::make_pair(CrawledUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		//
		// Link Problems Storages
		//

		std::make_pair(ExternalUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(UpperCaseUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(NonAsciiCharacterUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(VeryLongUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(Status404StorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		//
		// Title Storages
		//

		std::make_pair(AllTitlesUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		std::make_pair(EmptyTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		std::make_pair(DuplicatedTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		std::make_pair(VeryLongTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		std::make_pair(VeryShortTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		std::make_pair(DuplicatedH1TitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(SeveralTitleUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherTitle), 
				PageRawComparatorProxy(new PageRawTitleComparator)))),

		//
		// Meta Description Storages
		//

		std::make_pair(AllMetaDescriptionsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		std::make_pair(EmptyMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		std::make_pair(DuplicatedMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		std::make_pair(VeryLongMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		std::make_pair(VeryShortMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		std::make_pair(SeveralMetaDescriptionUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaDescription), 
				PageRawComparatorProxy(new PageRawMetaDescriptionComparator)))),

		//
		// Meta Keywords Problems Storages
		//

		std::make_pair(AllMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaKeywords), 
				PageRawComparatorProxy(new PageRawMetaKeywordsComparator)))),

		std::make_pair(EmptyMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaKeywords), 
				PageRawComparatorProxy(new PageRawMetaKeywordsComparator)))),

		std::make_pair(DuplicatedMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaKeywords), 
				PageRawComparatorProxy(new PageRawMetaKeywordsComparator)))),

		std::make_pair(SeveralMetaKeywordsUrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherMetaKeywords), 
				PageRawComparatorProxy(new PageRawMetaKeywordsComparator)))),

		//
		// H1 Problems Storages
		//

		std::make_pair(AllH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH1), 
				PageRawComparatorProxy(new PageRawFirstH1Comparator)))),

		std::make_pair(MissingH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH1), 
				PageRawComparatorProxy(new PageRawFirstH1Comparator)))),

		std::make_pair(DuplicatedH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH1), 
				PageRawComparatorProxy(new PageRawFirstH1Comparator)))),

		std::make_pair(VeryLongH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH1), 
				PageRawComparatorProxy(new PageRawFirstH1Comparator)))),

		std::make_pair(SeveralH1UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH1), 
				PageRawComparatorProxy(new PageRawFirstH1Comparator)))),

		//
		// H2 Problems Storages
		//

		std::make_pair(AllH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH2), 
				PageRawComparatorProxy(new PageRawFirstH2Comparator)))),

		std::make_pair(MissingH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH2), 
				PageRawComparatorProxy(new PageRawFirstH2Comparator)))),

		std::make_pair(DuplicatedH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH2), 
				PageRawComparatorProxy(new PageRawFirstH2Comparator)))),

		std::make_pair(VeryLongH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH2), 
				PageRawComparatorProxy(new PageRawFirstH2Comparator)))),

		std::make_pair(SeveralH2UrlStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherFirstH2), 
				PageRawComparatorProxy(new PageRawFirstH2Comparator)))),

		//
		// Images Problems Storages
		//

		std::make_pair(Over100kbImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(MissingAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(VeryLongAltTextImageStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		//
		// Resources Types Storages
		//

		std::make_pair(PendingResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(HtmlResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ImageResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(JavaScriptResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(StyleSheetResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(FlashResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(VideoResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(OtherResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl), 
				PageRawComparatorProxy(new PageRawUrlComparator)))),


		std::make_pair(ExternalHtmlResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalImageResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalJavaScriptResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalStyleSheetResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalFlashResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalVideoResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),

		std::make_pair(ExternalOtherResourcesStorageType, CrawlerStorageTypePtr(
			new CrawlerStorageType(0, PageRawHasherProxy(new PageRawHasherUrl),
				PageRawComparatorProxy(new PageRawUrlComparator)))),
	};

	m_guiStorage->initializeStorages();
}

}