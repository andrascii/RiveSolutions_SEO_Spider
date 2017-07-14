#pragma once

#include "gumbo.h"

namespace QuickieWebBot
{

class GumboParsingHelpers
{
public:
	static GumboNode* findSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent = 0) noexcept;

	static QString nodeText(const GumboNode* node) noexcept;

	static std::vector<GumboNode*> subNodes(const GumboNode* node, GumboTag tag) noexcept;

	static unsigned countChildren(const GumboNode* node, GumboTag tag) noexcept;
};

}
