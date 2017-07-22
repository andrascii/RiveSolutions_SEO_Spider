#pragma once

#include "ipage_parser.h"

namespace QuickieWebBot
{

class HtmlPageHParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageInfoPtr& pageInfo) noexcept override;

private:
	void recurseSearch(const GumboNode* node, GumboTag tag, std::vector<QString>& container) noexcept;

private:
	std::vector<QString> m_h1Tags;
	std::vector<QString> m_h2Tags;
};

}
