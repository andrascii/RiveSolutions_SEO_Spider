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
			p->descriptions.push_back({ CrawlerEngine::StorageType::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::Status404StorageType, QObject::tr("Broken Links") });

			return p;
		}

		case AuditGroup::TitleAuditGroup:
		{
			p->name = QObject::tr("Titles");
			p->group = AuditGroup::TitleAuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });

			return p;
		}

		case AuditGroup::MetaDescriptionAuditGroup:
		{
			p->name = QObject::tr("Meta Descriptions");
			p->group = AuditGroup::MetaDescriptionAuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });

			return p;
		}

		case AuditGroup::MetaKeywordAuditGroup:
		{
			p->name = QObject::tr("Meta Keywords");
			p->group = AuditGroup::MetaKeywordAuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty  Meta Keywords") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated  Meta Keywords") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->name = QObject::tr("H1");
			p->group = AuditGroup::H1AuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1") });

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->name = QObject::tr("H2");
			p->group = AuditGroup::H2AuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2") });

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->name = QObject::tr("Images");
			p->group = AuditGroup::ImageAuditGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::Over100kbImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VeryLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->name = QObject::tr("Internal Resources");
			p->group = AuditGroup::InternalResourcesGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::VideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::OtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->name = QObject::tr("External Resources");
			p->group = AuditGroup::ExternalResourcesGroup;
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back({ CrawlerEngine::StorageType::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}
	}

	return nullptr;
}

}