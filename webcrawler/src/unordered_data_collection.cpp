#include "unordered_data_collection.h"
#include "parsed_page_comparator.h"
#include "sequenced_data_collection.h"

namespace WebCrawler
{

UnorderedDataCollection::UnorderedDataCollection(QObject* parent, QThread* sequencedDataCollectionThread)
	: QObject(parent)
	, m_sequencedDataCollection(new SequencedDataCollection)
{
	initializeStorages();

	QThread* mainThread = QApplication::instance() != nullptr ? QApplication::instance()->thread() : nullptr;

	sequencedDataCollectionThread = sequencedDataCollectionThread != nullptr ?
		sequencedDataCollectionThread : mainThread;

	ASSERT(sequencedDataCollectionThread != nullptr);
	
	m_sequencedDataCollection->moveToThread(sequencedDataCollectionThread);

	VERIFY(connect(this, &UnorderedDataCollection::parsedPageAdded, m_sequencedDataCollection, 
		&SequencedDataCollection::addParsedPage, Qt::QueuedConnection));

	VERIFY(connect(this, &UnorderedDataCollection::parsedPageLinksToThisResourceChanged, m_sequencedDataCollection,
		&SequencedDataCollection::parsedPageLinksToThisResourceChanged, Qt::QueuedConnection));
}

UnorderedDataCollection::~UnorderedDataCollection()
{
	m_sequencedDataCollection->deleteLater();
}

bool UnorderedDataCollection::isParsedPageExists(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept
{
	checkStorageType(type);

	const UnorderedStorageTypePtr& unorderedStorage = storage(type);
	return unorderedStorage->find(parsedPagePtr) != unorderedStorage->end();
}

const ParsedPagePtr UnorderedDataCollection::parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept
{
	checkStorageType(type);

	const UnorderedStorageTypePtr& unorderedStorage = storage(type);
	auto iter = unorderedStorage->find(parsedPagePtr);

	return iter != unorderedStorage->end() ? *iter : ParsedPagePtr();
}

WebCrawler::SequencedDataCollection* UnorderedDataCollection::sequencedDataCollection() const noexcept
{
	return m_sequencedDataCollection;
}

void UnorderedDataCollection::addParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept
{
	if (isParsedPageExists(parsedPagePtr, type))
	{
		ASSERT(storage(type)->size() > 0);
		auto iter = storage(type)->find(parsedPagePtr);
		return;
	}

	storage(type)->insert(parsedPagePtr);
	emit parsedPageAdded(parsedPagePtr, static_cast<int>(type));
}

ParsedPagePtr UnorderedDataCollection::removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept
{
	checkStorageType(type);

	const UnorderedStorageTypePtr& unorderedStorage = storage(type);
	auto iter = unorderedStorage->find(parsedPagePtr);

	if (iter != unorderedStorage->end())
	{
		ParsedPagePtr result = *iter;
		unorderedStorage->erase(iter);
		return result;
	}

	return ParsedPagePtr();
}

UnorderedDataCollection::UnorderedStorageTypePtr& UnorderedDataCollection::storage(StorageType type) noexcept
{
	ASSERT(m_unorderedStorageMap.find(type) != m_unorderedStorageMap.end());
	return m_unorderedStorageMap.find(type)->second;
}

const UnorderedDataCollection::UnorderedStorageTypePtr& UnorderedDataCollection::storage(StorageType type) const noexcept
{
	const UnorderedStorageTypePtr& storage = const_cast<UnorderedDataCollection* const>(this)->storage(type);
	return storage;
}

void UnorderedDataCollection::checkStorageType(StorageType type) const noexcept
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


void UnorderedDataCollection::initializeStorages()
{
	m_unorderedStorageMap = std::initializer_list<std::pair<const int, UnorderedStorageTypePtr>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		//
		// Link Problems Storages
		//

		std::make_pair(StorageType::ExternalUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::UpperCaseUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::Status404StorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		//
		// Title Storages
		//

		std::make_pair(StorageType::AllTitlesUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator)))),

		std::make_pair(StorageType::EmptyTitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator)))),

		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongTitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator)))),

		std::make_pair(StorageType::VeryShortTitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator)))),

		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::SeveralTitleUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator)))),

		//
		// Meta Description Storages
		//

		std::make_pair(StorageType::AllMetaDescriptionsUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator)))),

		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator)))),

		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongMetaDescriptionUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator)))),

		std::make_pair(StorageType::VeryShortMetaDescriptionUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator)))),

		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator)))),

		//
		// Meta Keywords Problems Storages
		//

		std::make_pair(StorageType::AllMetaKeywordsUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator)))),

		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator)))),

		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator)))),

		//
		// H1 Problems Storages
		//

		std::make_pair(StorageType::AllH1UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator)))),

		std::make_pair(StorageType::MissingH1UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator)))),

		std::make_pair(StorageType::DuplicatedH1UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongH1UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator)))),

		std::make_pair(StorageType::SeveralH1UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator)))),

		//
		// H2 Problems Storages
		//

		std::make_pair(StorageType::AllH2UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator)))),

		std::make_pair(StorageType::MissingH2UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator)))),

		std::make_pair(StorageType::DuplicatedH2UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongH2UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator)))),

		std::make_pair(StorageType::SeveralH2UrlStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator)))),

		//
		// Images Problems Storages
		//

		std::make_pair(StorageType::Over100kbImageStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::MissingAltTextImageStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VeryLongAltTextImageStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		//
		// Resources Types Storages
		//

		std::make_pair(StorageType::PendingResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::HtmlResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ImageResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::JavaScriptResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::StyleSheetResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::FlashResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::VideoResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::OtherResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),


		std::make_pair(StorageType::ExternalHtmlResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalImageResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalFlashResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalVideoResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),

		std::make_pair(StorageType::ExternalOtherResourcesStorageType, UnorderedStorageTypePtr(
			new UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator)))),
	};

	m_sequencedDataCollection->initializeStorages();
}

}