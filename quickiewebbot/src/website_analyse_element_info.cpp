#include "website_analyse_element_info.h"

namespace QuickieWebBot
{

QString WebsiteAnalyseElementInfo::title(ElementInfo item)
{
	checkInfoItem(item);

	static std::map<ElementInfo, QString> s_titles
	{
		{ Url, "Url" },
		{ Title, "Title" }
	};

	return s_titles[item];
}

QVariant WebsiteAnalyseElementInfo::value(WebSiteAnalyseElementPtr element, ElementInfo item)
{
	return acceptElement(item)(element);
}

WebsiteAnalyseElementInfo::GetterInternal WebsiteAnalyseElementInfo::acceptElement(ElementInfo item)
{
	checkInfoItem(item);

	switch (item)
	{
		case QuickieWebBot::WebsiteAnalyseElementInfo::Url:
			return &WebsiteAnalyseElementInfo::elementUrl;

		case QuickieWebBot::WebsiteAnalyseElementInfo::Title:
			return &WebsiteAnalyseElementInfo::elementTitle;
	}

	//
	// TODO: handle this warning!!!
	//
	return GetterInternal();
}

QVariant WebsiteAnalyseElementInfo::elementUrl(WebSiteAnalyseElementPtr element)
{
	return element->url.toString();
}

QVariant WebsiteAnalyseElementInfo::elementTitle(WebSiteAnalyseElementPtr element)
{
	return element->title;
}

void WebsiteAnalyseElementInfo::checkInfoItem(ElementInfo item)
{
	assert(item == Url || item == Title);
}

}