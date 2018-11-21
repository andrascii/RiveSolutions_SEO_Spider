#pragma once

namespace CrawlerEngine
{

class CurlHttpHeadersWrapper final
{
public:
	CurlHttpHeadersWrapper();
	~CurlHttpHeadersWrapper();

	void add(const std::string& name, const std::string& value);

	curl_slist* get() const;

private:
	curl_slist* m_head;
};

}