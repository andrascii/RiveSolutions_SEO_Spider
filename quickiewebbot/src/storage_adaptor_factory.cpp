#include "storage_adaptor_factory.h"
#include "storage_adaptor.h"
#include "page_info.h"

namespace QuickieWebBot
{

StorageAdaptorFactory::StorageAdaptorFactory(DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
{
}

StorageAdaptor* StorageAdaptorFactory::create(DataCollection::StorageType type)
{
	assert(type > DataCollection::BeginEnumStorageType && type < DataCollection::EndEnumStorageType);

	if (m_storageAdaptors.find(type) != std::end(m_storageAdaptors))
	{
		return m_storageAdaptors[type];
	}

	StorageAdaptor* storageAdaptor = new StorageAdaptor(m_dataCollection->guiStorage(type), type, m_dataCollection);

	setupAvailableColumns(storageAdaptor, type);

	m_storageAdaptors[type] = storageAdaptor;

	return storageAdaptor;
}

void StorageAdaptorFactory::setupAvailableColumns(StorageAdaptor* storageAdaptor, DataCollection::StorageType type) const
{
	switch (type)
	{
		case DataCollection::CrawledUrlStorageType:
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
		case DataCollection::UpperCaseUrlStorageType:
		case DataCollection::NonAsciiCharacterUrlStorageType:
		case DataCollection::VeryLongUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::UrlLengthItemType
				<< PageInfo::ContentItemType
				<< PageInfo::StatusCodeItemType
			);

			return;
		}

		//
		// Title available columns
		//
		case DataCollection::EmptyTitleUrlStorageType:
		case DataCollection::DuplicatedTitleUrlStorageType:
		case DataCollection::VeryLongTitleUrlStorageType:
		case DataCollection::VeryShortTitleUrlStorageType:
		case DataCollection::DuplicatedH1TitleUrlStorageType:
		case DataCollection::SeveralTitleUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::TitleItemType
				<< PageInfo::TitleLengthItemType
			);

			return;
		}

		//
		// Meta description available columns
		//
		case DataCollection::EmptyMetaDescriptionUrlStorageType:
		case DataCollection::DuplicatedMetaDescriptionUrlStorageType:
		case DataCollection::VeryLongMetaDescriptionUrlStorageType:
		case DataCollection::VeryShortMetaDescriptionUrlStorageType:
		case DataCollection::SeveralMetaDescriptionUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaDescriptionItemType
				<< PageInfo::MetaDescriptionLengthItemType
			);

			return;
		}

		//
		// Meta keywords available columns
		// 
		case DataCollection::EmptyMetaKeywordsUrlStorageType:
		case DataCollection::DuplicatedMetaKeywordsUrlStorageType:
		case DataCollection::SeveralMetaKeywordsUrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::MetaKeywordsItemType
				<< PageInfo::MetaKeywordsLengthItemType
			);

			return;
		}

		//
		// H1 available columns
		//
		case DataCollection::MissingH1UrlStorageType:
		case DataCollection::DuplicatedH1UrlStorageType:
		case DataCollection::VeryLongH1UrlStorageType:
		case DataCollection::SeveralH1UrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH1ItemType
				<< PageInfo::FirstH1LengthItemType
				<< PageInfo::SecondH1ItemType
				<< PageInfo::SecondH1LengthItemType
			);

			return;
		}

		//
		// H2 available columns
		//
		case DataCollection::MissingH2UrlStorageType:
		case DataCollection::DuplicatedH2UrlStorageType:
		case DataCollection::VeryLongH2UrlStorageType:
		case DataCollection::SeveralH2UrlStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ContentItemType
				<< PageInfo::FirstH2ItemType
				<< PageInfo::FirstH2LengthItemType
				<< PageInfo::SecondH2ItemType
				<< PageInfo::SecondH2LengthItemType
			);

			return;
		}

		//
		// Images available columns
		//
		case DataCollection::Over100kbImageStorageType:
		case DataCollection::MissingAltTextImageStorageType:
		case DataCollection::VeryLongAltTextImageStorageType:
		{
			storageAdaptor->setAvailableColumns(QList<PageInfo::ItemType>()
				<< PageInfo::UrlItemType
				<< PageInfo::ImageAltTextItemType
				<< PageInfo::ImageAltTextLengthItemType
				<< PageInfo::ImageSizeKbItemType
			);

			return;
		}
	}
}

}