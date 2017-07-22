#pragma once

#include "gumbo.h"
#include "page_info.h"

namespace QuickieWebBot
{

class IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageInfoPtr& pageInfo) noexcept = 0;
};

}