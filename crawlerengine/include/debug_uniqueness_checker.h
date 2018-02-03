#pragma once

#include "iuniqueness_checker.h"

namespace CrawlerEngine
{

class DebugUniquenessChecker : public IUniquenessChecker
{
public:
	virtual bool hasRequest(const CrawlerRequest& request) const noexcept override;
	virtual void registrateRequest(const CrawlerRequest& request) override;

private:
	std::unordered_set<CrawlerRequest, CrawlerRequestHasher> m_alreadyLoadedLinks;
};

}