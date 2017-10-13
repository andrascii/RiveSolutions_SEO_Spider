#pragma once
#include "parsed_page.h"

namespace WebCrawler
{
	
struct LinksToThisResourceChanges
{
	struct Change
	{
		ParsedPagePtr page;
		size_t fromIndex;
	};
	std::vector<Change> changes;
};

Q_DECLARE_METATYPE(LinksToThisResourceChanges);

}
