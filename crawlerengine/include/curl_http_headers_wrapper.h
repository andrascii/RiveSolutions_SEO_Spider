#pragma once

namespace CrawlerEngine
{

class CurlHttpHeadersWrapper final
{
public:
	CurlHttpHeadersWrapper();
	~CurlHttpHeadersWrapper();

	void add(const QByteArray& name, const QByteArray& value);

	curl_slist* get() const;

private:
	curl_slist* m_head;
};

}