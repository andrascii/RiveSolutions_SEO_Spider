#include "universal_page_info_hasher.h"

namespace QuickieWebBot
{

UniversalPageInfoHasher::UniversalPageInfoHasher(const std::shared_ptr<IPageInfoHasher>& hasher)
	: m_hasher(hasher)
{
}

size_t UniversalPageInfoHasher::operator()(const PageInfoPtr& pageInfo) const noexcept
{
	return (*m_hasher)(pageInfo);
}

}