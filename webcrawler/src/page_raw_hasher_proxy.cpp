#include "page_raw_hasher_proxy.h"

namespace WebCrawler
{

PageRawHasherProxy::PageRawHasherProxy(const std::shared_ptr<IPageRawHasher>& hasher)
	: m_hasher(hasher)
{
}

size_t PageRawHasherProxy::operator()(const PageRawPtr& pageRaw) const noexcept
{
	return (*m_hasher)(pageRaw);
}

}