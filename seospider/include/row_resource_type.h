#pragma once
#include "parsed_page.h"

namespace SeoSpider
{

enum RowResourceType
{
	ResourceNone = 0,
	ResourceHtml = 1,
	ResourceImage = 1 << 1,
	ResourceJavaScript = 1 << 2,
	ResourceStyleSheet = 1 << 3,
	ResourceFlash = 1 << 4,
	ResourceVideo = 1 << 5,
	ResourceOther = 1 << 6,

	ResourceAny = ResourceHtml | ResourceImage | ResourceJavaScript | ResourceStyleSheet | ResourceFlash | ResourceVideo | ResourceOther,
};

inline RowResourceType resourceTypeToRowResourceType(CrawlerEngine::ResourceType resourceType)
{
	using RT = CrawlerEngine::ResourceType;
	switch(resourceType)
	{
	case RT::ResourceHtml:
		return ResourceHtml;
	case RT::ResourceImage:
		return ResourceImage;
	case RT::ResourceJavaScript:
		return ResourceJavaScript;
	case RT::ResourceStyleSheet:
		return ResourceStyleSheet;
	case RT::ResourceFlash:
		return ResourceFlash;
	case RT::ResourceVideo:
		return ResourceVideo;
	case RT::ResourceOther:
		return ResourceOther;
	}

	return ResourceNone;
}

}
