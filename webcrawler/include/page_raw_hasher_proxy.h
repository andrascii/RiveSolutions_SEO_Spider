#pragma once

#include "page_raw_hasher.h"

namespace WebCrawler
{

class PageRawHasherProxy
{
public:
	PageRawHasherProxy(IPageRawHasher* hasher);
	size_t operator()(const PageRawPtr& pageRaw) const noexcept;

private:
	std::shared_ptr<IPageRawHasher> m_hasher;
};

}
