#include "filter_info_factory.h"
#include "storage_adapter_type.h"

namespace
{

using namespace SeoSpider;

std::map<StorageAdapterType, FilterInfo> s_filters
{
	{
		StorageAdapterType::StorageAdapterTypeUpperCaseLinks,
		{
			QObject::tr("Links with upper case characters"),
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
			QObject::tr("This filter stores links that exceed the length specified in the program settings. (Settings => Limits Settings)")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeBrokenLinks,
		{
			QObject::tr("Broken links"),
			QObject::tr("This filter stores broken links. Links with server response code \"404 Not Found\".")
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
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeEmptyTitles,
		{
			QObject::tr("Pages with empty titles"),
			QObject::tr("This filter stores links to pages with empty title tags or without it.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedTitles,
		{
			QObject::tr("Pages with duplicated titles"),
			QObject::tr("This filter stores links to pages with duplicated titles.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooLongTitles,
		{
			QObject::tr("Pages with too long titles"),
			QObject::tr("This filter stores links to pages with too long titles. You can change this value (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooShortTitles,
		{
			QObject::tr("Pages with too short titles"),
			QObject::tr("This filter stores links to pages with too short titles. You can change this value (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles,
		{
			QObject::tr("Pages with titles that duplicate the tag H1"),
			QObject::tr("This filter contains pages with titles that duplicate the H1 tag.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage,
		{
			QObject::tr("Pages with several title tags"),
			QObject::tr("This filter stores links to pages with several title tags on the same page.")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions,
		{
			QObject::tr("Pages with empty meta descriptions"),
			QObject::tr("This filter stores links to pages with empty meta descriptions or without it.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions,
		{
			QObject::tr("Pages with duplicated meta descriptions"),
			QObject::tr("This filter stores links to pages with duplicated meta descriptions.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions,
		{
			QObject::tr("Pages with too long meta descriptions"),
			QObject::tr("This filter stores links to pages with too long meta descriptions. You can change this length in the settings (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions,
		{
			QObject::tr("Pages with too long meta descriptions"),
			QObject::tr("This filter stores links to pages with too short meta descriptions. You can change this length in the settings (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage,
		{
			QObject::tr("Pages with with several meta description tags"),
			QObject::tr("This filter stores links to pages with several meta descriptions tags.")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords,
		{
			QObject::tr("Pages with empty meta keywords"),
			QObject::tr("This filter stores links to pages with empty meta keywords or without this tag.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords,
		{
			QObject::tr("Pages with duplicated meta keywords"),
			QObject::tr("This filter stores links to pages with duplicated meta keywords.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage,
		{
			QObject::tr("Pages with with several meta keywords tags"),
			QObject::tr("This filter stores links to pages with several meta keywords tags.")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeMissingH1s,
		{
			QObject::tr("Pages with missing H1 tag"),
			QObject::tr("This filter stores links to pages with missing H1 tag.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedH1s,
		{
			QObject::tr("Pages with duplicated H1 tags"),
			QObject::tr("This filter stores links to pages duplicated H1 tags. H1 tags compares on different pages.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooLongH1s,
		{
			QObject::tr("Pages with too long H1 tags"),
			QObject::tr("This filter stores links to pages with too long H1 tags. You can change this length in the settings (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralH1s,
		{
			QObject::tr("Pages with several H1 tags on the same page"),
			QObject::tr("This filter stores links to pages with several H1 tags on the same page.")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeMissingH2s,
		{
			QObject::tr("Pages with missing H2 tag"),
			QObject::tr("This filter stores links to pages with missing H2 tag.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeDuplicatedH2s,
		{
			QObject::tr("Pages with duplicated H2 tags"),
			QObject::tr("This filter stores links to pages duplicated H2 tags. H2 tags compares on different pages.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooLongH2s,
		{
			QObject::tr("Pages with too long H2 tags"),
			QObject::tr("This filter stores links to pages with too long H2 tags. You can change this length in the settings (Settings => Preferences Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralH2s,
		{
			QObject::tr("Pages with several H2 tags on the same page"),
			QObject::tr("This filter stores links to pages with several H2 tags on the same page.")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeImagesOver100kb,
		{
			QObject::tr("Images over 100 kilobytes"),
			QObject::tr("This filter stores links to images that over 100 kilobytes. You can change this value in settings (Settings => Preferences Settings).")
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
			QObject::tr("This filter stores links to images with too long alt text. You can change this value in settings (Settings => Preferences Settings).")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeTooManyLinksOnPage,
		{
			QObject::tr("Too many links on page"),
			QObject::tr("This filter stores links to pages with too many links on page. You can change this value in settings (Settings => Limits Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeContainsMetaRefreshTag,
		{
			QObject::tr("Pages with meta refresh tag"),
			QObject::tr("This filter stores links to pages with meta refresh tag.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeContainsFrames,
		{
			QObject::tr("Images with too long alt text"),
			QObject::tr("This filter stores links to images with too long alt text. You can change this value in settings (Settings => Preferences Settings).")
		}
	},
	//////////////////////////////////////////////////////////////////////////
	{
		StorageAdapterType::StorageAdapterTypeBlockedForSEIndexing,
		{
			QObject::tr("Blocked for search engine indexing pages"),
			QObject::tr("This filter stores links to blocked for search engine indexing pages. This filter accumulate all blocked pages.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeNofollowLinks,
		{
			QObject::tr("Nofollow links"),
			QObject::tr("This filter stores nofollow links. Each link here has rel=\"nofollow\" attribute on all pages.")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeBlockedByRobotsTxt,
		{
			QObject::tr("Blocked by robots.txt pages"),
			QObject::tr("This filter stores blocked by robots.txt pages if you turned on "
				"\"Follow robots.txt rules\" in the \"Settings => Crawler Settings\".\n"
				"Additionally you can choose robot to follow rules for it in the \"Settings => User Agent Settings\".")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeBlockedByXRobotsTag,
		{
			QObject::tr("Pages blocked by x-robot-tag or meta robots"),
			QObject::tr("This filter stores links to pages blocked by \"<meta name=\"robots\" content=\"something\">\" depending on robot you chose or x-robots-tag.")
		}
	}
};

}

namespace SeoSpider
{

std::optional<FilterInfo> FilterInfoFactory::filterInfo(StorageAdapterType filter)
{
	auto it = s_filters.find(filter);

	if (it != s_filters.end())
	{
		return it->second;
	}

	return std::optional<FilterInfo>();
}

}
