#include "sequenced_storage.h"

namespace CrawlerEngine
{

bool SequencedStorage::containsPointersWithUseCountGreaterThanOne() const noexcept
{
	int pagesWithUseCountGreaterThanOne = 0;
	for (const ParsedPagePtr& pagePointer : m_pages)
	{
		if (pagePointer.use_count() > 1)
		{
			pagesWithUseCountGreaterThanOne++;
		}
	}

	if (pagesWithUseCountGreaterThanOne > 0)
	{
		ERRORLOG << "Some pages will not be destroyed, count: " << pagesWithUseCountGreaterThanOne;
		return true;
	}

	return false;
}

}