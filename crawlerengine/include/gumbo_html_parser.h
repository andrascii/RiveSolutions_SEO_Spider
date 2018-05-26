#pragma once

#include "gumbo.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

enum class ResourceSource;

class GumboHtmlParser final : public IHtmlParser
{
public:
	GumboHtmlParser(const GumboOptions* options, const QByteArray& htmlPage);
	virtual ~GumboHtmlParser();

	virtual QByteArray identifyHtmlPageContentType() const override;
	virtual QByteArray decodeHtmlPage(const ResponseHeaders& headers) const override;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const override;

private:
	LinkInfo getLinkRelUrl(const GumboNode* node, const char* relValue, ResourceSource source) const;

private:
	const GumboOptions* m_gumboOptions;
	GumboOutput* m_gumboOutput;
	QByteArray m_htmlPage;
};

}