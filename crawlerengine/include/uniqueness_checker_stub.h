#pragma once

namespace CrawlerEngine
{

class UniquenessCheckerStub : public IUniquenessChecker
{
public:
	virtual bool hasRequest(const CrawlerRequest& request) const noexcept override
	{
		Q_UNUSED(request);
		return false;
	}
	virtual void registrateRequest(const CrawlerRequest& request) override
	{
		Q_UNUSED(request);
	}
};

}