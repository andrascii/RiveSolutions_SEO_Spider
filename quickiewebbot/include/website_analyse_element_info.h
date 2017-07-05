#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class WebsiteAnalyseElementInfo
{
public:
	enum ElementInfo 
	{ 
		Url,
		Title
	};

	static QString title(ElementInfo item);
	static QVariant value(WebSiteAnalyseElementPtr element, ElementInfo item);

private: 
	using GetterInternal = QVariant(*)(WebSiteAnalyseElementPtr);

	static GetterInternal acceptElement(ElementInfo item);
	static QVariant elementUrl(WebSiteAnalyseElementPtr element);
	static QVariant elementTitle(WebSiteAnalyseElementPtr element);

	static void checkInfoItem(ElementInfo item);
};

}