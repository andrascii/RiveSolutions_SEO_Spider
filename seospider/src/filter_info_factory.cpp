#include "filter_info_factory.h"
#include "storage_adapter_type.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

std::optional<FilterInfo> FilterInfoFactory::filterInfo(StorageAdapterType filter)
{
	auto it = filters().find(filter);

	if (it != filters().end())
	{
		return it->second;
	}

	return std::optional<FilterInfo>();
}

std::map<StorageAdapterType, FilterInfo>& FilterInfoFactory::filters()
{
	static std::map<StorageAdapterType, FilterInfo> s_filters
	{
		{
			StorageAdapterType::StorageAdapterTypeUpperCaseLinks,
			{
				QObject::tr("Links with upper-case characters"),
				QObject::tr("This filter stores links that contain upper-case characters.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeNonAsciiLinks,
			{
				QObject::tr("Links with non-ascii characters"),
				QObject::tr("This filter stores links that contain non-ascii characters.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooLongLinks,
			{
				QObject::tr("Too long links"),
				QObject::tr("This filter stores links that exceed %1 characters. This value can be changed in the \"Settings => Limits Settings\""),
				{ "limitMaxUrlLength" }
			},
		},
		{
			StorageAdapterType::StorageAdapterTypeBrokenLinks,
			{
				QObject::tr("Broken links"),
				QObject::tr("This filter stores broken links with the server status code \"404 Not Found\".")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeStatus4xx,
			{
				QObject::tr("Links with 4xx status code"),
				QObject::tr("This filter stores links with the server status code 4xx.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeStatus5xx,
			{
				QObject::tr("Links with 5xx status code"),
				QObject::tr("This filter stores links with the server status code 5xx.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeStatus302,
			{
				QObject::tr("Redirection links with status code 302"),
				QObject::tr("This filter stores links with the server status code 302.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeStatus301,
			{
				QObject::tr("Redirection links with status code 301"),
				QObject::tr("This filter stores links with the server status code 301.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooManyRedirections,
			{
				QObject::tr("Links with too many redirections"),
				QObject::tr("This filter stores links with too many redirections (links with %1 or more redirections).\n"
					"You can change this value in the \"Settings => Limit Settings\"."),
				{ "maxRedirectCount" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTimeout,
			{
				QObject::tr("Timeout links"),
				QObject::tr("This filter stores links that exceed maximum timeout load time (%1 milliseconds).\n"
					"You can change this value in the \"Settings => Limit Settings\"."),
				{ "limitTimeout" }
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeEmptyTitles,
			{
				QObject::tr("Web-pages with empty titles"),
				QObject::tr("This filter stores links to web-pages with empty or missing title tags.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedTitles,
			{
				QObject::tr("Web-pages with duplicated titles"),
				QObject::tr("This filter stores links to web-pages with duplicated titles.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooLongTitles,
			{
				QObject::tr("Web-pages with too long titles"),
				QObject::tr("This filter stores links to web-pages with too long titles (links that contain more than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "maxTitleLength" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooShortTitles,
			{
				QObject::tr("Web-pages with too short titles"),
				QObject::tr("This filter stores links to web-pages with too short titles (links that contain less than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "minTitleLength" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles,
			{
				QObject::tr("Web-pages with titles that duplicate H1 tags"),
				QObject::tr("This filter contains web-pages with titles that duplicate H1 tags.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage,
			{
				QObject::tr("Web-pages with several title tags"),
				QObject::tr("This filter stores links to web-pages with several title tags on the same page.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions,
			{
				QObject::tr("Web-pages with empty meta descriptions"),
				QObject::tr("This filter stores links to web-pages with empty or missing meta descriptions.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions,
			{
				QObject::tr("Web-pages with duplicated meta descriptions"),
				QObject::tr("This filter stores links to web-pages with duplicated meta descriptions.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions,
			{
				QObject::tr("Web-pages with too long meta descriptions"),
				QObject::tr("This filter stores links to web-pages with too long meta descriptions (descriptions that contain more than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "maxDescriptionLength" }
			},
		},
		{
			StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions,
			{
				QObject::tr("Web-pages with too short meta descriptions"),
				QObject::tr("This filter stores links to web-pages with too short meta descriptions (descriptions that contain less than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "minDescriptionLength" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage,
			{
				QObject::tr("Web-pages with with several meta description tags"),
				QObject::tr("This filter stores links to web-pages with several meta descriptions tags.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords,
			{
				QObject::tr("Web-pages with empty meta keywords"),
				QObject::tr("This filter stores links to web-pages with empty or missing meta keywords tag.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords,
			{
				QObject::tr("Web-pages with duplicated meta keywords"),
				QObject::tr("This filter stores links to web-pages with duplicated meta keywords.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage,
			{
				QObject::tr("Web-pages with with several meta keywords tags"),
				QObject::tr("This filter stores links to web-pages with several meta keywords tags.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeMissingH1s,
			{
				QObject::tr("Web-pages with missing H1 tag"),
				QObject::tr("This filter stores links to web-pages with missing H1 tag.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedH1s,
			{
				QObject::tr("Web-pages with duplicated H1 tags"),
				QObject::tr("This filter stores links to web-pages with duplicated H1 tags. \n"
					"H1 tags comparison are accomplished on different web-pages.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooLongH1s,
			{
				QObject::tr("Web-pages with too long H1 tags"),
				QObject::tr("This filter stores links to web-pages with too long H1 tags (H1 tags that contain more than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "maxH1LengthChars" }
			},
		},
		{
			StorageAdapterType::StorageAdapterTypeSeveralH1s,
			{
				QObject::tr("Web-pages with several H1 tags on the same page"),
				QObject::tr("This filter stores links to web-pages with several H1 tags on the same page.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeMissingH2s,
			{
				QObject::tr("Web-pages with missing H2 tag"),
				QObject::tr("This filter stores links to web-pages with missing H2 tag.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeDuplicatedH2s,
			{
				QObject::tr("Web-pages with duplicated H2 tags"),
				QObject::tr("This filter stores links to web-pages with duplicated H2 tags. \n"
					"H2 tags comparison are accomplished on different web-pages.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeTooLongH2s,
			{
				QObject::tr("Web-pages with too long H2 tags"),
				QObject::tr("This filter stores links to web-pages with too long H2 tags (H2 tags that contain more than %1 characters).\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "maxH2LengthChars" }
			},
		},
		{
			StorageAdapterType::StorageAdapterTypeSeveralH2s,
			{
				QObject::tr("Web-pages with several H2 tags on the same page"),
				QObject::tr("This filter stores links to web-pages with several H2 tags on the same page.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeImagesOver100kb,
			{
				QObject::tr("Images over 100 kilobytes"),
				QObject::tr("this filter stores links to images that are bigger than %1 kilobytes.\n"
					"You can change this value in the \"Settings => Preferences Settings\"."),
				{ "maxImageSize" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeImageMissingAltText,
			{
				QObject::tr("Images with missing alt text"),
				QObject::tr("This filter stores links to images with missing alt text.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeImagesTooLongAltText,
			{
				QObject::tr("Images with too long alt text"),
				QObject::tr("This filter stores links to images with too long alt text (alt text that contains more than %1 characters).\n"
					"You can change this value in \"Settings => Preferences Settings\"."),
				{ "maxImageAltTextChars" }
			},
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeTooManyLinksOnPage,
			{
				QObject::tr("Too many links on web-page"),
				QObject::tr("This filter stores links to web-pages that contain too many links (more than %1).\n"
					"You can change this value in \"Settings => Limits Settings\"."),
				{ "maxLinksCountOnPage" }
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeContainsMetaRefreshTag,
			{
				QObject::tr("Web-pages with meta refresh tag"),
				QObject::tr("This filter stores links to web-pages that contain meta refresh tag.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeContainsFrames,
			{
				QObject::tr("Web-pages with frame tags"),
				QObject::tr("This filter stores links to web-pages that contain frame tags.")
			}
		},
		//////////////////////////////////////////////////////////////////////////
		{
			StorageAdapterType::StorageAdapterTypeBlockedForSEIndexing,
			{
				QObject::tr("Blocked for search engine indexing web-pages"),
				QObject::tr("This filter stores links to blocked for search engine indexing web-pages.\n"
					"This filter accumulates all blocked pages.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeNofollowLinks,
			{
				QObject::tr("Nofollow links"),
				QObject::tr("This filter stores nofollow links. Each link has rel=\"nofollow\" attribute on all web-pages.")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeBlockedByRobotsTxt,
			{
				QObject::tr("Blocked by robots.txt web-pages"),
				QObject::tr("This filter stores blocked by robots.txt web-pages but only if you turned on the option "
					"\"Follow robots.txt rules\" in the \"Settings => Crawler Settings\".\n"
					"Additionally you can choose specific robot's rules to follow in the \"Settings => User Agent Settings\".")
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeBlockedByXRobotsTag,
			{
				QObject::tr("Web-pages blocked by x-robot-tag or meta robots"),
				QObject::tr("This filter stores links to web-pages blocked by\n"
					"\"<meta name=\"robots\" content=\"something\">\" or x-robots-tag response header.\n"
					"Additionally you can choose specific robot's rules to follow in the \"Settings => User Agent Settings\".")
			}
		},
		// yandex metrica counters
		{
			StorageAdapterType::StorageAdapterTypeYandexMetricaCounter1,
			{
				QObject::tr("Pages with missing Yandex Metrica Counter"),
				QObject::tr("This filter stores pages that did not find the yandex metrica counter with the number %1.\n"
				"You can change the number of this counter in the settings \"Settings => Yandex Metrica Settings\".").arg(theApp->preferences()->yandexMetricaCounter1Id())
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeYandexMetricaCounter2,
			{
				QObject::tr("Pages with missing Yandex Metrica Counter"),
				QObject::tr("This filter stores pages that did not find the yandex metrica counter with the number %1.\n"
					"You can change the number of this counter in the settings "
					"\"Settings => Yandex Metrica Settings\".").arg(theApp->preferences()->yandexMetricaCounter2Id())
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeYandexMetricaCounter3,
			{
				QObject::tr("Pages with missing Yandex Metrica Counter"),
				QObject::tr("This filter stores pages that did not find the yandex metrica counter with the number %1.\n"
					"You can change the number of this counter in the settings "
					"\"Settings => Yandex Metrica Settings\".").arg(theApp->preferences()->yandexMetricaCounter3Id())
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeYandexMetricaCounter4,
			{
				QObject::tr("Pages with missing Yandex Metrica Counter"),
				QObject::tr("This filter stores pages that did not find the yandex metrica counter with the number %1.\n"
					"You can change the number of this counter in the settings "
					"\"Settings => Yandex Metrica Settings\".").arg(theApp->preferences()->yandexMetricaCounter4Id())
			}
		},
		{
			StorageAdapterType::StorageAdapterTypeYandexMetricaCounter5,
			{
				QObject::tr("Pages with missing Yandex Metrica Counter"),
				QObject::tr("This filter stores pages that did not find the yandex metrica counter with the number %1.\n"
					"You can change the number of this counter in the settings "
					"\"Settings => Yandex Metrica Settings\".").arg(theApp->preferences()->yandexMetricaCounter5Id())
			}
		},
	};

	return s_filters;
}

}
