#include "parsed_page_hasher_proxy.h"

namespace CrawlerEngine
{

ParsedPageHasherProxy::ParsedPageHasherProxy(IParsedPageHasher* hasher)
	: m_hasher(hasher)
{
}

size_t ParsedPageHasherProxy::operator()(const ParsedPagePtr& parsedPage) const noexcept
{
	return (*m_hasher)(parsedPage);
}

}