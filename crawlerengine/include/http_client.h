#pragma once

#include "ihttp_client.h"

namespace CrawlerEngine
{

class HttpClient : public IHttpClient
{
public:
	HttpClient();

	virtual HopsChain get(const Url& url) override;
	virtual HopsChain post(const Url& url) override;

private:
	static CURL* initCurl();

private:
	CURL* m_connection;
};

}
