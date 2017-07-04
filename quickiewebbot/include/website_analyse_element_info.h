#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class WebsiteAnalyseElementInfo
{
public:
	enum Info 
	{ 
		Url,
		Title
	};

	static QString getTitle(Info item);
	static QVariant getValue(WebSiteAnalyseElementPtr element, Info item);

private: 
	using GetterInternal = QVariant(*)(WebSiteAnalyseElementPtr);

	static GetterInternal getter(Info item);

	static QVariant getElementUrl(WebSiteAnalyseElementPtr element);

	static QVariant getElementTitle(WebSiteAnalyseElementPtr element)
	{
		return element->title;
	}
};

}