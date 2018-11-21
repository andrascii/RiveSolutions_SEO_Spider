#pragma once

#include "hops_chain.h"

namespace CrawlerEngine
{

class IHttpClient
{
public:
	virtual ~IHttpClient() = default;

	virtual HopsChain get(const Url& url) = 0;
	virtual HopsChain post(const Url& url) = 0;
};

}
