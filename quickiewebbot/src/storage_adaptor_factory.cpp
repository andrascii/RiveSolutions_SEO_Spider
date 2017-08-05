#include "storage_adaptor_factory.h"
#include "storage_adaptor.h"
#include "page_info.h"

namespace QuickieWebBot
{

StorageAdaptorFactory::StorageAdaptorFactory(DataCollection* dataCollection)
	: m_dataCollection(dataCollection)
{
}

StorageAdaptor* StorageAdaptorFactory::create(DataCollection::StorageType type) const
{
	assert(type > DataCollection::BeginEnumStorageType && type < DataCollection::EndEnumStorageType);

	switch (type)
	{
		//
		// Statistic data
		//
		case DataCollection::CrawledUrlStorageType:
		{
			StorageAdaptor* storageAdaptor = new StorageAdaptor(m_dataCollection->guiStorage(type), type, m_dataCollection);

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

			return storageAdaptor;
		}
		case DataCollection::ExternalUrlStorageType:
		{

		}

		//
		// Url string problems
		//
		case DataCollection::UpperCaseUrlStorageType:
		{

		}
		case DataCollection::NonAsciiCharacterUrlStorageType:
		{

		}
		case DataCollection::VeryLongUrlStorageType:
		{

		}

		//
		// Titles problems
		//
		case DataCollection::EmptyTitleUrlStorageType:
		{

		}
		case DataCollection::DuplicatedTitleUrlStorageType:
		{

		}
		case DataCollection::VeryLongTitleUrlStorageType:
		{

		}
		case DataCollection::VeryShortTitleUrlStorageType:
		{

		}
		case DataCollection::DuplicatedH1TitleUrlStorageType:
		{

		}
		case DataCollection::SeveralTitleUrlStorageType:
		{

		}

		//
		// Meta Descriptions problems
		//
		case DataCollection::EmptyMetaDescriptionUrlStorageType:
		{

		}
		case DataCollection::DuplicatedMetaDescriptionUrlStorageType:
		{

		}
		case DataCollection::VeryLongMetaDescriptionUrlStorageType:
		{

		}
		case DataCollection::VeryShortMetaDescriptionUrlStorageType:
		{

		}
		case DataCollection::SeveralMetaDescriptionUrlStorageType:
		{

		}

		//
		// Meta Kerywords problems
		//
		case DataCollection::EmptyMetaKeywordsUrlStorageType:
		{

		}
		case DataCollection::DuplicatedMetaKeywordsUrlStorageType:
		{

		}
		case DataCollection::SeveralMetaKeywordsUrlStorageType:
		{

		}

		//
		// H1 problems
		//
		case DataCollection::MissingH1UrlStorageType:
		{

		}
		case DataCollection::DuplicatedH1UrlStorageType:
		{

		}
		case DataCollection::VeryLongH1UrlStorageType:
		{

		}
		case DataCollection::SeveralH1UrlStorageType:
		{

		}

		//
		// H2 problems
		//
		case DataCollection::MissingH2UrlStorageType:
		{

		}
		case DataCollection::DuplicatedH2UrlStorageType:
		{

		}
		case DataCollection::VeryLongH2UrlStorageType:
		{

		}
		case DataCollection::SeveralH2UrlStorageType:
		{

		}

		//
		// Images problems
		//
		case DataCollection::Over100kbImageStorageType:
		{

		}
		case DataCollection::MissingAltTextImageStorageType:
		{

		}
		case DataCollection::VeryLongAltTextImageStorageType:
		{

		}
	}

	assert(!"Invalid type");
	return nullptr;
}

}