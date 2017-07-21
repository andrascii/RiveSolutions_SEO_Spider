#pragma once

#include "page_info_hasher.h"

namespace QuickieWebBot
{

class UniversalPageInfoHasher
{
public:
	UniversalPageInfoHasher(const std::shared_ptr<IPageInfoHasher>& hasher);
	size_t operator()(const PageInfoPtr& pageInfo) const noexcept;

private:
	std::shared_ptr<IPageInfoHasher> m_hasher;
};

}
