#include "resources_cache.h"

namespace WebCrawler
{
	
void ResourcesCache::clear()
{
	m_resources.clear();
}

bool ResourcesCache::isResourceExists(const QUrl& url) const
{
	if (m_resources.find(url.toDisplayString()) != m_resources.cend())
	{
		return true;
	}

	m_resources.insert(url.toDisplayString());
	return false;
}

}