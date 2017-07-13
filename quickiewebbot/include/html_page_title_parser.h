#pragma once

#include "ipage_parser.h"

namespace QuickieWebBot
{

class HtmlPageTitleParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageInfoPtr& pageInfo) noexcept override;
};

}