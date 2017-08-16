#include "universal_page_raw_hasher.h"

namespace WebCrawler
{

UniversalPageRawHasher::UniversalPageRawHasher(const std::shared_ptr<IPageRawHasher>& hasher)
	: m_hasher(hasher)
{
}

size_t UniversalPageRawHasher::operator()(const PageRawPtr& pageRaw) const noexcept
{
	return (*m_hasher)(pageRaw);
}

}