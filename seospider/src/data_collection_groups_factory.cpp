#include "data_collection_groups_factory.h"
#include "icustom_data_feed.h"
#include "preferences.h"
#include "application.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(AuditGroup group) const
{
	std::shared_ptr<DCStorageGroupDescription> p =
		std::make_shared<DCStorageGroupDescription>();

	switch (group)
	{
		case AuditGroup::LinkAuditGroup:
		{
			p->name = QObject::tr("Link Problems");
			p->auditGroup = AuditGroup::LinkAuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BrokenLinks, QObject::tr("Broken Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status4xxStorageType, QObject::tr("Status Code 4xx") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status5xxStorageType, QObject::tr("Status Code 5xx") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status302StorageType, QObject::tr("Moved Temporarily 302") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status301StorageType, QObject::tr("Moved Permanently 301") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooManyRedirectsStorageType, QObject::tr("Too Many Redirections") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TimeoutStorageType, QObject::tr("Timeout") });

			return p;
		}

		case AuditGroup::OnPageAuditGroup:
		{
			p->name = QObject::tr("On Page Problems");
			p->auditGroup = AuditGroup::OnPageAuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty Meta Keywords") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated Meta Keywords") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });

			return p;
		}

		case AuditGroup::NotIndexedPagesGroup:
		{
			p->name = QObject::tr("Blocked for Indexing Pages");
			p->auditGroup = AuditGroup::NotIndexedPagesGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedForSEIndexingStorageType, QObject::tr("All Not Indexed Pages") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::NofollowLinksStorageType, QObject::tr("Nofollow Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedByRobotsTxtStorageType, QObject::tr("Blocked by robots.txt Pages") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedByXRobotsTagStorageType, QObject::tr("Blocked by x-robots-tag Pages") });

			return p;
		}

		case AuditGroup::PageProblemsAuditGroup:
		{
			p->name = QObject::tr("Page Problems");
			p->auditGroup = AuditGroup::OnPageAuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooManyLinksOnPageStorageType, QObject::tr("Too Many Links On Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ContainsMetaRefreshTagStorageType, QObject::tr("Pages Contain Meta Refresh Tag") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ContainsFramesStorageType, QObject::tr("Pages Contain Frames") });

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->name = QObject::tr("H1 Problems");
			p->auditGroup = AuditGroup::H1AuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1 On Page") });

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->name = QObject::tr("H2 Problems");
			p->auditGroup = AuditGroup::H2AuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2 On Page") });

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->name = QObject::tr("Image Problems");
			p->auditGroup = AuditGroup::ImageAuditGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooBigImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });

			return p;
		}

		case AuditGroup::OrderedErrorsGroup:
		{
			p->name = QObject::tr("Error Groups");
			p->auditGroup = AuditGroup::OrderedErrorsGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::UpperCaseUrlStorageType, QObject::tr("Links With Uppercase Characters") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links With Non-ASCII Characters") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongUrlStorageType, QObject::tr("Too Long Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BrokenLinks, QObject::tr("Broken Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status4xxStorageType, QObject::tr("Status Code 4xx") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status5xxStorageType, QObject::tr("Status Code 5xx") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status302StorageType, QObject::tr("Moved Temporarily 302") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::Status301StorageType, QObject::tr("Moved Permanently 301") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TimeoutStorageType, QObject::tr("Timeout") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooManyRedirectsStorageType, QObject::tr("Too Many Redirections") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty Meta Keywords") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated Meta Keywords") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooManyLinksOnPageStorageType, QObject::tr("Too Many Links On Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ContainsMetaRefreshTagStorageType, QObject::tr("Pages Contain Meta Refresh Tag") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ContainsFramesStorageType, QObject::tr("Pages Contain Frames") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongH1UrlStorageType, QObject::tr("Too Long H1") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1 On Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongH2UrlStorageType, QObject::tr("Too Long H2") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2 On Page") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooBigImageStorageType, QObject::tr("Images Over 100 KB") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::MissingAltTextImageStorageType, QObject::tr("Images With Missing Alt Description") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::TooLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedForSEIndexingStorageType, QObject::tr("All Not Indexed Pages") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::NofollowLinksStorageType, QObject::tr("Nofollow Links") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedByRobotsTxtStorageType, QObject::tr("Blocked by robots.txt Pages") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::BlockedByXRobotsTagStorageType, QObject::tr("Blocked by x-robots-tag Pages") });

			const auto yandexMetricaDescriptions = createYandexMetricaDescriptions();
			p->descriptions.insert(p->descriptions.end(), yandexMetricaDescriptions.begin(), yandexMetricaDescriptions.end());

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->name = QObject::tr("Internal Resources");
			p->auditGroup = AuditGroup::InternalResourcesGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::VideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::OtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->name = QObject::tr("External Resources");
			p->auditGroup = AuditGroup::ExternalResourcesGroup;
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			p->descriptions.push_back(DCStorageDescription{ StorageType::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });

			return p;
		}

		case AuditGroup::YandexMetricaCounters:
		{
			if (!theApp->preferences()->searchYandexMetricaCounters())
			{
				return p;
			}

			p->name = QObject::tr("Yandex Metrica");
			p->auditGroup = AuditGroup::YandexMetricaCounters;
			p->descriptions = createYandexMetricaDescriptions();

			return p;
		}
	}

	return nullptr;
}

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(const QVector<CrawlerEngine::ICustomDataFeed*> dataFeeds) const
{
	std::shared_ptr<DCStorageGroupDescription> p =
		std::make_shared<DCStorageGroupDescription>();

	p->name = QObject::tr("Custom Analisys");
	p->auditGroup = AuditGroup::CustomDataFeeds;

	foreach(const CrawlerEngine::ICustomDataFeed* dataFeed, dataFeeds)
	{
		DCStorageDescription description { CrawlerEngine::StorageType::CrawledUrlStorageType, dataFeed->name() };
		description.customDataFeed = dataFeed->name();
		p->descriptions.emplace_back(description);
	}

	return p;
}

std::vector<DCStorageDescription> DataCollectionGroupsFactory::createYandexMetricaDescriptions() const
{
	std::vector<DCStorageDescription> descriptions;

	if (!theApp->preferences()->searchYandexMetricaCounters())
	{
		return descriptions;
	}

	if (theApp->preferences()->searchYandexMetricaCounter1())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter1StorageType,
				QObject::tr(QString("Pages without Yandex Metrica %1").arg(theApp->preferences()->yandexMetricaCounter1Id()).toStdString().c_str())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter2())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter2StorageType,
				QObject::tr(QString("Pages without Yandex Metrica %1").arg(theApp->preferences()->yandexMetricaCounter2Id()).toStdString().c_str())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter3())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter3StorageType,
				QObject::tr(QString("Pages without Yandex Metrica %1").arg(theApp->preferences()->yandexMetricaCounter3Id()).toStdString().c_str())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter4())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter4StorageType,
				QObject::tr(QString("Pages without Yandex Metrica %1").arg(theApp->preferences()->yandexMetricaCounter4Id()).toStdString().c_str())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter5())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter5StorageType,
				QObject::tr(QString("Pages without Yandex Metrica %1").arg(theApp->preferences()->yandexMetricaCounter5Id()).toStdString().c_str())
			});
	}

	return descriptions;
}

}
