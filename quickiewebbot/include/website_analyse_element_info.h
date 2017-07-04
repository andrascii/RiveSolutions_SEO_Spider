#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class WebsiteAnalyseElementInfo {

public:
	enum Info 
	{ 
		Url,
		Title
	};

	static QString getTitle(Info item)
	{
		static std::map<Info, QString> s_titles{
			{ Url, "Url" },
			{ Title, "Title" }
		};

		return s_titles[item];
	}

	static QVariant getValue(WebSiteAnalyseElementPtr element, Info item)
	{
		return getter(item)(element);
	}

private: 
	using GetterInternal = QVariant(*)(WebSiteAnalyseElementPtr);

	static GetterInternal getter(Info item)
	{
		switch (item)
		{
		case QuickieWebBot::WebsiteAnalyseElementInfo::Url:
			return &WebsiteAnalyseElementInfo::getElementUrl;
			break;
		case QuickieWebBot::WebsiteAnalyseElementInfo::Title:
			return &WebsiteAnalyseElementInfo::getElementTitle;
			break;
		default:
			break;
		}
	}

	static QVariant getElementUrl(WebSiteAnalyseElementPtr element)
	{
		return element->url.toString();
	}

	static QVariant getElementTitle(WebSiteAnalyseElementPtr element)
	{
		return element->title;
	}
};

}