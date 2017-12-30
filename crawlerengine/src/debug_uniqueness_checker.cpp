#include "debug_uniqueness_checker.h"

namespace CrawlerEngine
{

bool DebugUniquenessChecker::hasRequest(const CrawlerRequest& request) const noexcept
{
	return m_alreadyLoadedLinks.find(request) != m_alreadyLoadedLinks.end();
}

void DebugUniquenessChecker::registrateRequest(const CrawlerRequest& request)
{
	m_alreadyLoadedLinks.insert(request);
}

}