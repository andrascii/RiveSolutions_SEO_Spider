#pragma once

#include "parsed_page_hasher.h"

namespace CrawlerEngine
{

class ParsedPageHasherProxy
{
public:
	ParsedPageHasherProxy(IParsedPageHasher* hasher);
	size_t operator()(const ParsedPagePtr& parsedPage) const noexcept;

private:
	std::shared_ptr<IParsedPageHasher> m_hasher;
};

}
