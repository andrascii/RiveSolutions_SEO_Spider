#include "storage_adaptor_factory.h"
#include "storage_adaptor.h"
#include "page_info.h"

namespace QuickieWebBot
{

StorageAdaptorFactory::StorageAdaptorFactory(WebCrawler::DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
{
}

StorageAdaptor* StorageAdaptorFactory::create(WebCrawler::DataCollection::StorageType type)
{
	assert(type > WebCrawler::DataCollection::BeginEnumStorageType && type < WebCrawler::DataCollection::EndEnumStorageType);

	if (m_storageAdaptors.find(type) != std::end(m_storageAdaptors))
	{
		return m_storageAdaptors[type];
	}

	StorageAdaptor* storageAdaptor = new StorageAdaptor(m_dataCollection->guiStorage(type), type, m_dataCollection);

	setupAvailableColumns(storageAdaptor, type);

	m_storageAdaptors[type] = storageAdaptor;

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(StorageAdaptor* storageAdaptor, WebCrawler::DataCollection::StorageType type) const
{
	switch (type)
	{
		case WebCrawler::DataCollection::CrawledUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::TitleItemType
				<< PageInfo::TitleLengthItemType
				<< PageInfo::MetaRefreshItemType
				<< PageInfo::MetaRobotsItemType
				<< PageInfo::MetaDescriptionItemType
				<< PageInfo::MetaDescriptionLengthItemType
				<< PageInfo::MetaKeywordsItemType
				<< PageInfo::MetaKeywordsLengthItemType
				<< PageInfo::RedirectedUrlItemType
				<< PageInfo::FirstH1ItemType
				<< PageInfo::FirstH1LengthItemType
				<< PageInfo::SecondH1ItemType
				<< PageInfo::SecondH1LengthItemType
				<< PageInfo::FirstH2ItemType
				<< PageInfo::FirstH2LengthItemType
				<< PageInfo::SecondH2ItemType
				<< PageInfo::SecondH2LengthItemType
				<< PageInfo::CanonicalLinkElementItemType
				<< PageInfo::StatusCodeItemType
				<< PageInfo::PageSizeKbItemType
				<< PageInfo::WordCountItemType
				<< PageInfo::PageHashItemType
				<< PageInfo::UrlLengthItemType
			);

			return;
		}

		//
		// Links available columns
		//
		case WebCrawler::DataCollection::UpperCaseUrlStorageType:
		case WebCrawler::DataCollection::NonAsciiCharacterUrlStorageType:
		case WebCrawler::DataCollection::VeryLongUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::UrlLengthItemType
				<< PageInfo::ContentItemType
				<< PageInfo::StatusCodeItemType
			);
		}

		//
		// Title available columns
		//
		case WebCrawler::DataCollection::EmptyTitleUrlStorageType:
		case WebCrawler::DataCollection::DuplicatedTitleUrlStorageType:
		case WebCrawler::DataCollection::VeryLongTitleUrlStorageType:
		case WebCrawler::DataCollection::VeryShortTitleUrlStorageType:
		case WebCrawler::DataCollection::DuplicatedH1TitleUrlStorageType:
		case WebCrawler::DataCollection::SeveralTitleUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::TitleItemType
				<< PageInfo::TitleLengthItemType
			);
		}

		//
		// Meta description available columns
		//
		case WebCrawler::DataCollection::EmptyMetaDescriptionUrlStorageType:
		case WebCrawler::DataCollection::DuplicatedMetaDescriptionUrlStorageType:
		case WebCrawler::DataCollection::VeryLongMetaDescriptionUrlStorageType:
		case WebCrawler::DataCollection::VeryShortMetaDescriptionUrlStorageType:
		case WebCrawler::DataCollection::SeveralMetaDescriptionUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaDescriptionItemType
				<< PageInfo::MetaDescriptionLengthItemType
			);
		}

		//
		// Meta keywords available columns
		// 
		case WebCrawler::DataCollection::EmptyMetaKeywordsUrlStorageType:
		case WebCrawler::DataCollection::DuplicatedMetaKeywordsUrlStorageType:
		case WebCrawler::DataCollection::SeveralMetaKeywordsUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaKeywordsItemType
				<< PageInfo::MetaKeywordsLengthItemType
			);
		}

		//
		// H1 available columns
		//
		case WebCrawler::DataCollection::MissingH1UrlStorageType:
		case WebCrawler::DataCollection::DuplicatedH1UrlStorageType:
		case WebCrawler::DataCollection::VeryLongH1UrlStorageType:
		case WebCrawler::DataCollection::SeveralH1UrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH1ItemType
				<< PageInfo::FirstH1LengthItemType
				<< PageInfo::SecondH1ItemType
				<< PageInfo::SecondH1LengthItemType
			);
		}

		//
		// H2 available columns
		//
		case WebCrawler::DataCollection::MissingH2UrlStorageType:
		case WebCrawler::DataCollection::DuplicatedH2UrlStorageType:
		case WebCrawler::DataCollection::VeryLongH2UrlStorageType:
		case WebCrawler::DataCollection::SeveralH2UrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH2ItemType
				<< PageInfo::FirstH2LengthItemType
				<< PageInfo::SecondH2ItemType
				<< PageInfo::SecondH2LengthItemType
			);
		}

		//
		// Images available columns
		//
		case WebCrawler::DataCollection::Over100kbImageStorageType:
		case WebCrawler::DataCollection::MissingAltTextImageStorageType:
		case WebCrawler::DataCollection::VeryLongAltTextImageStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ImageAltTextItemType
				<< PageInfo::ImageAltTextLengthItemType
				<< PageInfo::ImageSizeKbItemType
			);
		}
	}
}

}