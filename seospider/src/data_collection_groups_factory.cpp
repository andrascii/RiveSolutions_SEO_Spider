#include "data_collection_groups_factory.h"

namespace SeoSpider
{

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(AuditGroup group) const
{
	std::shared_ptr<DCStorageGroupDescription> p =
		std::make_shared<DCStorageGroupDescription>();

	switch (group)
	{
		case AuditGroup::LinkAuditGroup:
		{
			p->name = QObject::tr("Links");
			p->group = AuditGroup::LinkAuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.push_back({ WebCrawler::StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.push_back({ WebCrawler::StorageType::Status404StorageType, QObject::tr("Broken Links") });

			return p;
		}

		case AuditGroup::TitleAuditGroup:
		{
			p->name = QObject::tr("Titles");
			p->group = AuditGroup::TitleAuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.push_back({ WebCrawler::StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });

			return p;
		}

		case AuditGroup::MetaDescriptionAuditGroup:
		{
			p->name = QObject::tr("Meta Descriptions");
			p->group = AuditGroup::MetaDescriptionAuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.push_back({ WebCrawler::StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });

			return p;
		}

		case AuditGroup::MetaKeywordAuditGroup:
		{
			p->name = QObject::tr("Meta Keywords");
			p->group = AuditGroup::MetaKeywordAuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty  Meta Keywords") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated  Meta Keywords") });
			p->descriptions.push_back({ WebCrawler::StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->name = QObject::tr("H1");
			p->group = AuditGroup::H1AuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.push_back({ WebCrawler::StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1") });

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->name = QObject::tr("H2");
			p->group = AuditGroup::H2AuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.push_back({ WebCrawler::StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.push_back({ WebCrawler::StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2") });

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->name = QObject::tr("Images");
			p->group = AuditGroup::ImageAuditGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::Over100kbImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.push_back({ WebCrawler::StorageType::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.push_back({ WebCrawler::StorageType::VeryLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->name = QObject::tr("Internal Resources");
			p->group = AuditGroup::InternalResourcesGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::VideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::OtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->name = QObject::tr("External Resources");
			p->group = AuditGroup::ExternalResourcesGroup;
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ WebCrawler::StorageType::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}
	}

	return nullptr;
}

}