#include "sequenced_data_collection.h"
#include "unordered_data_collection.h"
#include "sequenced_storage.h"
#include "crawler_shared_state.h"
#include "isequenced_storage.h"

namespace CrawlerEngine
{

SequencedDataCollection::SequencedDataCollection(const UnorderedDataCollection* collection)
{
	ASSERT(collection);

	VERIFY(connect(collection, SIGNAL(parsedPageAdded(ParsedPagePtr, StorageType)), this,
		SLOT(addParsedPage(ParsedPagePtr, StorageType)), Qt::QueuedConnection));

	VERIFY(connect(collection, SIGNAL(parsedPageAdded(WorkerResult, StorageType)), this,
		SLOT(addParsedPage(WorkerResult, StorageType)), Qt::QueuedConnection));

	VERIFY(connect(collection, &UnorderedDataCollection::parsedPageReplaced, this,
		&SequencedDataCollection::replaceParsedPage, Qt::QueuedConnection));

	VERIFY(connect(collection, SIGNAL(parsedPageRemoved(ParsedPagePtr, StorageType)), this,
		SLOT(onParsedPageRemoved(ParsedPagePtr, StorageType)), Qt::QueuedConnection));

	VERIFY(connect(collection, &UnorderedDataCollection::parsedPageLinksToThisResourceChanged, this,
		&SequencedDataCollection::parsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

	VERIFY(connect(collection, &UnorderedDataCollection::dataCleared, this,
		&SequencedDataCollection::onDataCleared, Qt::QueuedConnection));
}

bool SequencedDataCollection::empty() const noexcept
{
	return storage(StorageType::CrawledUrlStorageType)->empty();
}

ISequencedStorage* SequencedDataCollection::storage(StorageType type) noexcept
{
	ISequencedStorage* storage = const_cast<ISequencedStorage*>(static_cast<const SequencedDataCollection* const>(this)->storage(type));
	return storage;
}

const ISequencedStorage* SequencedDataCollection::storage(StorageType type) const noexcept
{
	auto iter = m_sequencedStorageMap.find(type);

	return iter != m_sequencedStorageMap.end() ? iter->second.get() : nullptr;
}

bool SequencedDataCollection::removePage(ParsedPage* parsedPage, StorageType type)
{
	const auto fakeDeleter = [](ParsedPage*) noexcept {};

	const ParsedPagePtr pointer(parsedPage, fakeDeleter);

	ISequencedStorage* sequencedStorage = storage(type);

	if (sequencedStorage)
	{
		const RemoveEffects removeEffects = sequencedStorage->remove(pointer);

		if (removeEffects.removedIndex != -1)
		{
			emit parsedPageRemoved(removeEffects.removedIndex, type);
			emit indicesRangeInvalidated(removeEffects.invalidatedIndicesRange, type);

			return true;
		}

		return false;
	}

	return false;
}

void SequencedDataCollection::setCustomDataFeeds(const QVector<ICustomDataFeed*> dataFeeds)
{
	m_customDataFeeds = dataFeeds;

	for (ICustomDataFeed* dataFeed : dataFeeds)
	{
		QObject* qObject = dynamic_cast<QObject*>(dataFeed);
		ASSERT(qObject != nullptr);

		qObject->disconnect(this);
		VERIFY(QObject::connect(qObject, SIGNAL(dataReady(ICustomDataFeedRow*)),
			this, SLOT(onCustomDataFeedRowReceived(ICustomDataFeedRow*)), Qt::QueuedConnection));
	}
}

const QVector<ICustomDataFeed*>& SequencedDataCollection::customDataFeeds() const
{
	return m_customDataFeeds;
}

std::shared_ptr<ISequencedStorage> SequencedDataCollection::createSequencedStorage() const
{
	return std::static_pointer_cast<ISequencedStorage>(std::make_shared<SequencedStorage>());
}

void SequencedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, StorageType type)
{
	auto storageIterator = m_sequencedStorageMap.find(type);

	if (type == StorageType::CrawledUrlStorageType)
	{
		CrawlerSharedState::instance()->incrementSequencedDataCollectionLinksCount();

		// TODO: check if page added first time
		if (type == CrawledUrlStorageType)
		{
			for (ICustomDataFeed* dataFeed : m_customDataFeeds)
			{
				if (dataFeed->connected())
				{
					dataFeed->requestData(parsedPagePtr);
				}
			}
		}
	}

	if (storageIterator != m_sequencedStorageMap.end())
	{
		auto&[storageType, collection] = *storageIterator;
		auto storageType1 = storageType;
		storageType1;

		collection->emplaceBack(std::move(parsedPagePtr));

		emit parsedPageAdded(collection->size() - 1, storageType);
	}
}

void SequencedDataCollection::addParsedPage(WorkerResult workerResult, StorageType type)
{
	addParsedPage(workerResult.incomingPage(), type);
}

void SequencedDataCollection::replaceParsedPage(ParsedPagePtr oldParsedPagePtr, ParsedPagePtr newParsedPagePtr, StorageType type)
{
	auto storageIterator = m_sequencedStorageMap.find(type);

	if (storageIterator != m_sequencedStorageMap.end())
	{
		auto&[storageType, collection] = *storageIterator;

		const int replacedIndex = collection->replace(std::move(oldParsedPagePtr), std::move(newParsedPagePtr));

		emit parsedPageReplaced(replacedIndex, storageType);
	}
}


void SequencedDataCollection::onParsedPageRemoved(ParsedPagePtr parsedPagePointer, StorageType type)
{
	removePage(parsedPagePointer.get(), type);
}

void SequencedDataCollection::onDataCleared()
{
	emit beginClearData();

	for (auto& [storageType, collection] : m_sequencedStorageMap)
	{
		if (storageType == StorageType::CrawledUrlStorageType)
		{
			continue;
		}

		collection->clear();
	}

	std::shared_ptr<ISequencedStorage>& sequencedStorage = m_sequencedStorageMap[StorageType::CrawledUrlStorageType];

	ASSERT(!sequencedStorage->containsPointersWithUseCountGreaterThanOne());

	sequencedStorage->clear();

	CrawlerSharedState::instance()->setSequencedDataCollectionLinksCount(0);

	emit endClearData();
}

void SequencedDataCollection::onCustomDataFeedRowReceived(ICustomDataFeedRow* row)
{
	if (ParsedPagePtr page = row->page().lock())
	{
		const QStringList columns = row->dataFeed()->columns();
		const DataFeedId feedId = row->dataFeed()->dataFeedId();

		int index = 0;
		foreach(const QString& column, columns)
		{
			page->dataFeedsData[feedId][index] = row->data(column);
			index += 1;
		}

		// TODO: optimize
		const auto storageIterator = m_sequencedStorageMap.find(CrawledUrlStorageType);
		const int pageIndex = storageIterator->second->find(page.get());
		// DEBUG_ASSERT(pageIndex >= 0);
		if (pageIndex >= 0)
		{
			emit indicesRangeInvalidated(std::make_pair(pageIndex, pageIndex), CrawledUrlStorageType);
		}

		//for (size_t i = 0; i < page->storages.size(); ++i)
		//{
		//	if (!page->storages[i])
		//	{
		//		continue;
		//	}
		//
		//	StorageType storageType = static_cast<StorageType>(i);
		//	const auto storageIterator = m_sequencedStorageMap.find(storageType);

		//	if (storageIterator != m_sequencedStorageMap.end())
		//	{
		//		const int pageIndex = storageIterator->second->find(page.get());
		//		// DEBUG_ASSERT(pageIndex >= 0);
		//		if (pageIndex >= 0)
		//		{
		//			emit indicesRangeInvalidated(std::make_pair(pageIndex, pageIndex), storageType);
		//		}
		//	}
		//}
	}
}

void SequencedDataCollection::initialize()
{
	m_sequencedStorageMap = std::initializer_list<std::pair<const StorageType, std::shared_ptr<ISequencedStorage>>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, createSequencedStorage()),

		// Link Problems Storages
		std::make_pair(StorageType::ExternalUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::UpperCaseUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::BrokenLinks, createSequencedStorage()),
		std::make_pair(StorageType::WwwRedirectionsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooManyRedirectsStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TimeoutStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status4xxStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status5xxStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status302StorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status301StorageType, createSequencedStorage()),

		// Title Storages
		std::make_pair(StorageType::EmptyTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooShortTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralTitleUrlStorageType, createSequencedStorage()),

		// Meta Description Storages
		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooShortMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, createSequencedStorage()),

		// Meta Keywords Problems Storages
		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, createSequencedStorage()),

		// H1 Problems Storages
		std::make_pair(StorageType::MissingH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH1UrlStorageType, createSequencedStorage()),

		// H2 Problems Storages
		std::make_pair(StorageType::MissingH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH2UrlStorageType, createSequencedStorage()),

		// Images Problems Storages
		std::make_pair(StorageType::TooBigImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::MissingAltTextImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongAltTextImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::BrokenImagesStorageType, createSequencedStorage()),

		// Page problems
		std::make_pair(StorageType::TooManyLinksOnPageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ContainsMetaRefreshTagStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ContainsFramesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooBigHtmlResourcesStorageType, createSequencedStorage()),

		// not indexed pages
		std::make_pair(StorageType::BlockedForSEIndexingStorageType, createSequencedStorage()),
		std::make_pair(StorageType::NofollowLinksStorageType, createSequencedStorage()),
		std::make_pair(StorageType::BlockedByRobotsTxtStorageType, createSequencedStorage()),
		std::make_pair(StorageType::BlockedByXRobotsTagStorageType, createSequencedStorage()),

		// Resources Types Storages
		std::make_pair(StorageType::HtmlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ImageResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::JavaScriptResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::StyleSheetResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::FlashResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VideoResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::OtherResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalHtmlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalImageResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalFlashResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalVideoResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalOtherResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::AllCanonicalUrlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedCanonicalUrlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::UniqueCanonicalUrlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalDoFollowUrlResourcesStorageType, createSequencedStorage()),

		// yandex metrica
		std::make_pair(StorageType::YandexMetricaCounter1StorageType, createSequencedStorage()),
		std::make_pair(StorageType::YandexMetricaCounter2StorageType, createSequencedStorage()),
		std::make_pair(StorageType::YandexMetricaCounter3StorageType, createSequencedStorage()),
		std::make_pair(StorageType::YandexMetricaCounter4StorageType, createSequencedStorage()),
		std::make_pair(StorageType::YandexMetricaCounter5StorageType, createSequencedStorage()),
	};
}

}
