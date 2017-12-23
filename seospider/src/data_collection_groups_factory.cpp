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
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::BrokenLinks, QObject::tr("Broken Links") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::Status4xxStorageType, QObject::tr("Status Code 4xx") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::Status5xxStorageType, QObject::tr("Status Code 5xx") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::Status302StorageType, QObject::tr("Moved Temporarily 302") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::Status301StorageType, QObject::tr("Moved Permanently 301") });

			return p;
		}

		case AuditGroup::OnPageAuditGroup:
		{
			p->name = QObject::tr("On Page");
			p->group = AuditGroup::OnPageAuditGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty Meta Keywords") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated Meta Keywords") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->name = QObject::tr("H1");
			p->group = AuditGroup::H1AuditGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1 On Page") });

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->name = QObject::tr("H2");
			p->group = AuditGroup::H2AuditGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2 On Page") });

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->name = QObject::tr("Images");
			p->group = AuditGroup::ImageAuditGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::Over100kbImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VeryLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->name = QObject::tr("Internal Resources");
			p->group = AuditGroup::InternalResourcesGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::VideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::OtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->name = QObject::tr("External Resources");
			p->group = AuditGroup::ExternalResourcesGroup;
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.emplace_back(DCStorageDescription{ CrawlerEngine::StorageType::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}
	}

	return nullptr;
}

}