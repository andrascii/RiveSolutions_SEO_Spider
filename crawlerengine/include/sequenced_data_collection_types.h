#pragma once
#include "parsed_page.h"

namespace CrawlerEngine
{
	
struct LinksToThisResourceChanges
{
	struct Change
	{
		ParsedPageWeakPtr page;
		size_t fromIndex;
	};
	std::vector<Change> changes;
};

}

Q_DECLARE_METATYPE(CrawlerEngine::LinksToThisResourceChanges);