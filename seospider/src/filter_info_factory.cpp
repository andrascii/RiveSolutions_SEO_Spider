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
			QObject::tr("This filter stores links that exceed the length specified in the program settings. (Settings => Crawler Settings)")
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
			QObject::tr("This filter stores links to pages with too long titles. You can change this value (Settings => Limits Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooShortTitles,
		{
			QObject::tr("Pages with too short titles"),
			QObject::tr("This filter stores links to pages with too short titles. You can change this value (Settings => Limits Settings).")
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
			QObject::tr("This filter stores links to pages with too long meta descriptions. You can change this length in the settings (Settings => Limits Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions,
		{
			QObject::tr("Pages with too long meta descriptions"),
			QObject::tr("This filter stores links to pages with too short meta descriptions. You can change this length in the settings (Settings => Limits Settings).")
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
			QObject::tr("This filter stores links to pages with too long H1 tags. You can change this length in the settings (Settings => Limits Settings).")
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
			QObject::tr("This filter stores links to pages with too long H2 tags. You can change this length in the settings (Settings => Limits Settings).")
		}
	},
	{
		StorageAdapterType::StorageAdapterTypeSeveralH2s,
		{
			QObject::tr("Pages with several H2 tags on the same page"),
			QObject::tr("This filter stores links to pages with several H2 tags on the same page.")
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
