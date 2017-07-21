#pragma once

#include "ipage_parser.h"

namespace QuickieWebBot
{

class HtmlPageMetaParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageInfoPtr& pageInfo, QTextCodec* textCodec = QTextCodec::codecForLocale()) noexcept override;
};

}
