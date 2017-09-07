#include "data_collection_groups_factory.h"

namespace QuickieWebBot
{

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(WebCrawler::DataCollection* dataCollection, AuditGroup group)
{
	std::shared_ptr<DCStorageGroupDescription> p =
		std::make_shared<DCStorageGroupDescription>();

	switch (group)
	{
		case AuditGroup::LinkAuditGroup:
		{
			p->name = QObject::tr("Links");
			p->group = AuditGroup::LinkAuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.push_back({ WebCrawler::DataCollection::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.push_back({ WebCrawler::DataCollection::Status404StorageType, QObject::tr("Broken Links") });

			return p;
		}

		case AuditGroup::TitleAuditGroup:
		{
			p->name = QObject::tr("Page's Titles");
			p->group = AuditGroup::TitleAuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });

			return p;
		}

		case AuditGroup::MetaDescriptionAuditGroup:
		{
			p->name = QObject::tr("Page's Meta Descriptions");
			p->group = AuditGroup::MetaDescriptionAuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });

			return p;
		}

		case AuditGroup::MetaKeywordAuditGroup:
		{
			p->name = QObject::tr("Page's Meta Keywords");
			p->group = AuditGroup::MetaKeywordAuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty  Meta Keywords") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated  Meta Keywords") });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->name = QObject::tr("Page's H1");
			p->group = AuditGroup::H1AuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralH1UrlStorageType, QObject::tr("Several Equal H1") });

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->name = QObject::tr("Page's H2");
			p->group = AuditGroup::H2AuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralH2UrlStorageType, QObject::tr("Several Equal H2") });

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->name = QObject::tr("Images");
			p->group = AuditGroup::ImageAuditGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::Over100kbImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->name = QObject::tr("Internal Resources");
			p->group = AuditGroup::InternalResourcesGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::VideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::OtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->name = QObject::tr("External Resources");
			p->group = AuditGroup::ExternalResourcesGroup;
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ WebCrawler::DataCollection::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}
	}

	return nullptr;
}

}