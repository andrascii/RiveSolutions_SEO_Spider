#pragma once

#include "crawler_request.h"

namespace CrawlerEngine
{

class IUniquenessChecker
{
public:
	virtual bool hasRequest(const CrawlerRequest& request) const noexcept = 0;
	virtual void registrateRequest(const CrawlerRequest& request) = 0;
};

std::unique_ptr<IUniquenessChecker> createUniquenessChecker();

}