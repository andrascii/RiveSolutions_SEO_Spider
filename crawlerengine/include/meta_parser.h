#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class MetaParser : public IPageParser
{
public:
	MetaParser();

	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	void parseMetaContentType(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaRefresh(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaDescription(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaKeywords(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaRobots(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& pageRaw) noexcept;

private:
	QRegularExpression m_metaRefreshContentPattern;
};

}
