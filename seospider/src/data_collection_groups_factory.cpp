#include "stdafx.h"
#include "data_collection_groups_factory.h"
#include "icustom_data_feed.h"
#include "preferences.h"
#include "application.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(AuditGroup group) const
{
	DCStorageGroupDescriptionPtr p = createDCStorageGroupDescriptionPtr();

	std::vector<DCStorageDescription> descriptions;

	switch (group)
	{
		case AuditGroup::LinkAuditGroup:
		{
			p->setName(QObject::tr("Link Problems"));
			p->setAuditGroup(AuditGroup::LinkAuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::UpperCaseUrlStorageType, QObject::tr("Links with Uppercase Characters") });
			descriptions.push_back(DCStorageDescription{ StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links with Non-ASCII Characters") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongUrlStorageType, QObject::tr("Too Long Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::BrokenLinks, QObject::tr("Broken Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status4xxStorageType, QObject::tr("Status Code 4xx") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status5xxStorageType, QObject::tr("Status Code 5xx") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status302StorageType, QObject::tr("Moved Temporarily 302") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status301StorageType, QObject::tr("Moved Permanently 301") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooManyRedirectsStorageType, QObject::tr("Too Many Redirections") });
			descriptions.push_back(DCStorageDescription{ StorageType::AllCanonicalUrlResourcesStorageType, QObject::tr("Canonical URLs") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedCanonicalUrlResourcesStorageType, QObject::tr("Duplicated Canonical URLs") });
			descriptions.push_back(DCStorageDescription{ StorageType::TimeoutStorageType, QObject::tr("Timeout") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::OnPageAuditGroup:
		{
			p->setName(QObject::tr("On Page Problems"));
			p->setAuditGroup(AuditGroup::OnPageAuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty Meta Keywords") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated Meta Keywords") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::NotIndexedPagesGroup:
		{
			p->setName(QObject::tr("Blocked for Indexing Pages"));
			p->setAuditGroup(AuditGroup::NotIndexedPagesGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedForSEIndexingStorageType, QObject::tr("All Not Indexed Pages") });
			descriptions.push_back(DCStorageDescription{ StorageType::NofollowLinksStorageType, QObject::tr("Nofollow Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedByRobotsTxtStorageType, QObject::tr("Blocked by robots.txt Pages") });
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedByXRobotsTagStorageType, QObject::tr("Blocked by x-robots-tag Pages") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::PageProblemsAuditGroup:
		{
			p->setName(QObject::tr("Page Problems"));
			p->setAuditGroup(AuditGroup::PageProblemsAuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::TooManyLinksOnPageStorageType, QObject::tr("Too Many Links On Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::ContainsMetaRefreshTagStorageType, QObject::tr("Pages Contain Meta Refresh Tag") });
			descriptions.push_back(DCStorageDescription{ StorageType::ContainsFramesStorageType, QObject::tr("Pages Contain Frames") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::H1AuditGroup:
		{
			p->setName(QObject::tr("H1 Problems"));
			p->setAuditGroup(AuditGroup::H1AuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongH1UrlStorageType, QObject::tr("Too Long H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1 On Page") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::H2AuditGroup:
		{
			p->setName(QObject::tr("H2 Problems"));
			p->setAuditGroup(AuditGroup::H2AuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongH2UrlStorageType, QObject::tr("Too Long H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2 On Page") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::ImageAuditGroup:
		{
			p->setName(QObject::tr("Image Problems"));
			p->setAuditGroup(AuditGroup::ImageAuditGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::TooBigImageStorageType, QObject::tr("Images Over 100 KB") });
			descriptions.push_back(DCStorageDescription{ StorageType::MissingAltTextImageStorageType, QObject::tr("Images with Missing Alt Description") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::OrderedErrorsGroup:
		{
			p->setName(QObject::tr("Error Groups"));
			p->setAuditGroup(AuditGroup::OrderedErrorsGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::UpperCaseUrlStorageType, QObject::tr("Links with Uppercase Characters") });
			descriptions.push_back(DCStorageDescription{ StorageType::NonAsciiCharacterUrlStorageType, QObject::tr("Links with Non-ASCII Characters") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongUrlStorageType, QObject::tr("Too Long Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::BrokenLinks, QObject::tr("Broken Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status4xxStorageType, QObject::tr("Status Code 4xx") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status5xxStorageType, QObject::tr("Status Code 5xx") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status302StorageType, QObject::tr("Moved Temporarily 302") });
			descriptions.push_back(DCStorageDescription{ StorageType::Status301StorageType, QObject::tr("Moved Permanently 301") });
			descriptions.push_back(DCStorageDescription{ StorageType::TimeoutStorageType, QObject::tr("Timeout") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooManyRedirectsStorageType, QObject::tr("Too Many Redirections") });
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyTitleUrlStorageType, QObject::tr("Empty Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedTitleUrlStorageType, QObject::tr("Duplicated Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongTitleUrlStorageType, QObject::tr("Too Long Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooShortTitleUrlStorageType, QObject::tr("Too Short Titles") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1TitleUrlStorageType, QObject::tr("Titles Duplicates H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralTitleUrlStorageType, QObject::tr("Several Title Tags On The Same Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaDescriptionUrlStorageType, QObject::tr("Empty Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaDescriptionUrlStorageType, QObject::tr("Duplicated Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongMetaDescriptionUrlStorageType, QObject::tr("Too Long Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooShortMetaDescriptionUrlStorageType, QObject::tr("Too Short Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaDescriptionUrlStorageType, QObject::tr("Several Meta Descriptions") });
			descriptions.push_back(DCStorageDescription{ StorageType::EmptyMetaKeywordsUrlStorageType, QObject::tr("Empty Meta Keywords") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedMetaKeywordsUrlStorageType, QObject::tr("Duplicated Meta Keywords") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralMetaKeywordsUrlStorageType, QObject::tr("Several Meta Keywords") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooManyLinksOnPageStorageType, QObject::tr("Too Many Links On Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::ContainsMetaRefreshTagStorageType, QObject::tr("Pages Contain Meta Refresh Tag") });
			descriptions.push_back(DCStorageDescription{ StorageType::ContainsFramesStorageType, QObject::tr("Pages Contain Frames") });
			descriptions.push_back(DCStorageDescription{ StorageType::MissingH1UrlStorageType, QObject::tr("Missing H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH1UrlStorageType, QObject::tr("Duplicated H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongH1UrlStorageType, QObject::tr("Too Long H1") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralH1UrlStorageType, QObject::tr("Several Equal H1 On Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::MissingH2UrlStorageType, QObject::tr("Missing H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedH2UrlStorageType, QObject::tr("Duplicated H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongH2UrlStorageType, QObject::tr("Too Long H2") });
			descriptions.push_back(DCStorageDescription{ StorageType::SeveralH2UrlStorageType, QObject::tr("Several Equal H2 On Page") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooBigImageStorageType, QObject::tr("Images Over 100 KB") });
			descriptions.push_back(DCStorageDescription{ StorageType::MissingAltTextImageStorageType, QObject::tr("Images with Missing Alt Description") });
			descriptions.push_back(DCStorageDescription{ StorageType::TooLongAltTextImageStorageType, QObject::tr("Too Long Image Alt Description") });
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedForSEIndexingStorageType, QObject::tr("All Not Indexed Pages") });
			descriptions.push_back(DCStorageDescription{ StorageType::NofollowLinksStorageType, QObject::tr("Nofollow Links") });
			descriptions.push_back(DCStorageDescription{ StorageType::AllCanonicalUrlResourcesStorageType, QObject::tr("Canonical URLs") });
			descriptions.push_back(DCStorageDescription{ StorageType::DuplicatedCanonicalUrlResourcesStorageType, QObject::tr("Duplicated Canonical URLs") });
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedByRobotsTxtStorageType, QObject::tr("Blocked by robots.txt Pages") });
			descriptions.push_back(DCStorageDescription{ StorageType::BlockedByXRobotsTagStorageType, QObject::tr("Blocked by x-robots-tag Pages") });

			const auto yandexMetricaDescriptions = createYandexMetricaDescriptions();
			descriptions.insert(descriptions.end(), yandexMetricaDescriptions.begin(), yandexMetricaDescriptions.end());

			const auto reconfigureYmDescriptions = [this, group = QPointer<DCStorageGroupDescription>(p.get())]
			{
				if (!group)
				{
					return;
				}

				group->setDescriptions(std::move(createYandexMetricaDescriptions()));
			};

			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter1Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter1IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter2Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter2IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter3Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter3IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter4Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter4IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter5Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter5IdChanged, p.get(), reconfigureYmDescriptions));

			const auto googleAnalyticsDescriptions = createGoogleAnalyticsDescriptions();
			descriptions.insert(descriptions.end(), googleAnalyticsDescriptions.begin(), googleAnalyticsDescriptions.end());
			p->setDescriptions(std::move(descriptions));

			const auto reconfigureGaDescriptions = [this, group = QPointer<DCStorageGroupDescription>(p.get())]
			{
				if (!group)
				{
					return;
				}

				group->setDescriptions(std::move(createGoogleAnalyticsDescriptions()));
			};

			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter1Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter1IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter2Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter2IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter3Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter3IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter4Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter4IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter5Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter5IdChanged, p.get(), reconfigureGaDescriptions));

			return p;
		}

		case AuditGroup::InternalResourcesGroup:
		{
			p->setName(QObject::tr("Internal Resources"));
			p->setAuditGroup(AuditGroup::InternalResourcesGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::HtmlResourcesStorageType, QObject::tr("HTML Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ImageResourcesStorageType, QObject::tr("Image Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::JavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::StyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::FlashResourcesStorageType, QObject::tr("Flash Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::VideoResourcesStorageType, QObject::tr("Video Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::OtherResourcesStorageType, QObject::tr("Other Resources") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::ExternalResourcesGroup:
		{
			p->setName(QObject::tr("External Resources"));
			p->setAuditGroup(AuditGroup::ExternalResourcesGroup);
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalHtmlResourcesStorageType, QObject::tr("HTML Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalImageResourcesStorageType, QObject::tr("Image Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalJavaScriptResourcesStorageType, QObject::tr("JavaScript Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalStyleSheetResourcesStorageType, QObject::tr("StyleSheet Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalFlashResourcesStorageType, QObject::tr("Flash Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalVideoResourcesStorageType, QObject::tr("Video Resources") });
			descriptions.push_back(DCStorageDescription{ StorageType::ExternalOtherResourcesStorageType, QObject::tr("Other Resources") });
			p->setDescriptions(std::move(descriptions));

			return p;
		}

		case AuditGroup::YandexMetricaCounters:
		{
			if (!theApp->preferences()->searchYandexMetricaCounters())
			{
				return nullptr;
			}

			p->setName(QObject::tr("Yandex Metrica Finder"));
			p->setAuditGroup(AuditGroup::YandexMetricaCounters);
			p->setDescriptions(std::move(createYandexMetricaDescriptions()));

			const auto reconfigureYmDescriptions = [this, group = QPointer<DCStorageGroupDescription>(p.get())]
			{
				if (!group)
				{
					return;
				}

				group->setDescriptions(std::move(createYandexMetricaDescriptions()));
			};

			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter1Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter1IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter2Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter2IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter3Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter3IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter4Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter4IdChanged, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCounter5Changed, p.get(), reconfigureYmDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::yandexMetricaCounter5IdChanged, p.get(), reconfigureYmDescriptions));

			return p;
		}

		case AuditGroup::GoogleAnalyticsCounters:
		{
			if (!theApp->preferences()->searchGoogleAnalyticsCounters())
			{
				return nullptr;
			}

			p->setName(QObject::tr("Google Analytics Finder"));
			p->setAuditGroup(AuditGroup::GoogleAnalyticsCounters);
			p->setDescriptions(std::move(createGoogleAnalyticsDescriptions()));

			const auto reconfigureGaDescriptions = [this, group = QPointer<DCStorageGroupDescription>(p.get())]
			{
				if (!group)
				{
					return;
				}

				group->setDescriptions(std::move(createGoogleAnalyticsDescriptions()));
			};

			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter1Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter1IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter2Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter2IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter3Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter3IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter4Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter4IdChanged, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchGoogleAnalyticsCounter5Changed, p.get(), reconfigureGaDescriptions));
			VERIFY(QObject::connect(theApp->preferences(), &Preferences::googleAnalyticsCounter5IdChanged, p.get(), reconfigureGaDescriptions));

			return p;
		}
	}

	return nullptr;
}

DCStorageGroupDescriptionPtr DataCollectionGroupsFactory::create(const QVector<CrawlerEngine::ICustomDataFeed*> dataFeeds) const
{
	DCStorageGroupDescriptionPtr p = createDCStorageGroupDescriptionPtr();

	std::vector<DCStorageDescription> descriptions;

	p->setName(QObject::tr("Custom Analisys"));
	p->setAuditGroup(AuditGroup::CustomDataFeeds);

	foreach(const CrawlerEngine::ICustomDataFeed* dataFeed, dataFeeds)
	{
		DCStorageDescription description { CrawlerEngine::StorageType::CrawledUrlStorageType, dataFeed->name() };
		description.customDataFeed = dataFeed->name();
		descriptions.emplace_back(description);
	}

	p->setDescriptions(std::move(descriptions));

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
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->yandexMetricaCounter1Id())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter2())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter2StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->yandexMetricaCounter2Id())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter3())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter3StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->yandexMetricaCounter3Id())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter4())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter4StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->yandexMetricaCounter4Id())
			});
	}

	if (theApp->preferences()->searchYandexMetricaCounter5())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::YandexMetricaCounter5StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->yandexMetricaCounter5Id())
			});
	}

	return descriptions;
}

std::vector<DCStorageDescription> DataCollectionGroupsFactory::createGoogleAnalyticsDescriptions() const
{
	std::vector<DCStorageDescription> descriptions;

	if (!theApp->preferences()->searchGoogleAnalyticsCounters())
	{
		return descriptions;
	}

	if (theApp->preferences()->searchGoogleAnalyticsCounter1())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::GoogleAnalyticsCounter1StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->googleAnalyticsCounter1Id())
			});
	}

	if (theApp->preferences()->searchGoogleAnalyticsCounter2())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::GoogleAnalyticsCounter2StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->googleAnalyticsCounter2Id())
			});
	}

	if (theApp->preferences()->searchGoogleAnalyticsCounter3())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::GoogleAnalyticsCounter3StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->googleAnalyticsCounter3Id())
			});
	}

	if (theApp->preferences()->searchGoogleAnalyticsCounter4())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::GoogleAnalyticsCounter4StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->googleAnalyticsCounter4Id())
			});
	}

	if (theApp->preferences()->searchGoogleAnalyticsCounter5())
	{
		descriptions.push_back(
			DCStorageDescription
			{
				StorageType::GoogleAnalyticsCounter5StorageType,
				QObject::tr("Pages without Counter %1").arg(theApp->preferences()->googleAnalyticsCounter5Id())
			});
	}

	return descriptions;
}

}
