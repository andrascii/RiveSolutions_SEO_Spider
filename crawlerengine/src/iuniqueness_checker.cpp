#pragma once

#include "debug_uniqueness_checker.h"
#include "uniqueness_checker_stub.h"

namespace CrawlerEngine
{

std::unique_ptr<IUniquenessChecker> createUniquenessChecker()
{
#ifdef PRODUCTION
	return std::make_unique<UniquenessCheckerStub>();
#else
	return std::make_unique<DebugUniquenessChecker>();
#endif
}

}