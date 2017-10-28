#pragma once

#include "parsed_page.h"
#include "gumbo.h"

namespace WebCrawler
{

class IPageParser
{
public:
	using ResponseHeaders = QList<QPair<QByteArray, QByteArray>>;

	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) = 0;
};

}
