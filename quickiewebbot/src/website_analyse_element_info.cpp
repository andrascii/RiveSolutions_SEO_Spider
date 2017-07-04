#include "website_analyse_element_info.h"

namespace QuickieWebBot
{

QString WebsiteAnalyseElementInfo::getTitle(Info item)
{
	checkInfoItem(item);

	static std::map<Info, QString> s_titles
	{
		{ Url, "Url" },
		{ Title, "Title" }
	};

	return s_titles[item];
}

QVariant WebsiteAnalyseElementInfo::getValue(WebSiteAnalyseElementPtr element, Info item)
{
	return getter(item)(element);
}

WebsiteAnalyseElementInfo::GetterInternal WebsiteAnalyseElementInfo::getter(Info item)
{
	checkInfoItem(item);

	switch (item)
	{
		case QuickieWebBot::WebsiteAnalyseElementInfo::Url:
			return &WebsiteAnalyseElementInfo::getElementUrl;

		case QuickieWebBot::WebsiteAnalyseElementInfo::Title:
			return &WebsiteAnalyseElementInfo::getElementTitle;
	}

	//
	// TODO: handle this warning!!!
	//
	return GetterInternal();
}

QVariant WebsiteAnalyseElementInfo::getElementUrl(WebSiteAnalyseElementPtr element)
{
	return element->url.toString();
}

QVariant WebsiteAnalyseElementInfo::getElementTitle(WebSiteAnalyseElementPtr element)
{
	return element->title;
}

void WebsiteAnalyseElementInfo::checkInfoItem(Info item)
{
	assert(item == Url || item == Title);
}

}