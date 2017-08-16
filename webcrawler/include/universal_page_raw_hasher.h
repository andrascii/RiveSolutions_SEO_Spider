#pragma once

#include "page_raw_hasher.h"

namespace WebCrawler
{

class UniversalPageRawHasher
{
public:
	UniversalPageRawHasher(const std::shared_ptr<IPageRawHasher>& hasher);
	size_t operator()(const PageRawPtr& pageRaw) const noexcept;

private:
	std::shared_ptr<IPageRawHasher> m_hasher;
};

}
