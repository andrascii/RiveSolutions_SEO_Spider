#pragma once

#include "gumbo.h"

namespace WebCrawler
{

class GumboOutputCreatorDestroyerGuard final
{
public:
	GumboOutputCreatorDestroyerGuard(const GumboOptions* options, const QByteArray& htmlPage);
	~GumboOutputCreatorDestroyerGuard();

	GumboOutputCreatorDestroyerGuard(const GumboOutputCreatorDestroyerGuard&) = delete;
	GumboOutputCreatorDestroyerGuard(GumboOutputCreatorDestroyerGuard&&) = delete;
	GumboOutputCreatorDestroyerGuard& operator=(const GumboOutputCreatorDestroyerGuard&) = delete;
	GumboOutputCreatorDestroyerGuard& operator=(GumboOutputCreatorDestroyerGuard&&) = delete;

	const GumboOutput* output() const noexcept;
	GumboOutput* output() noexcept;

private:
	const GumboOptions* m_gumboOptions;
	GumboOutput* m_gumboOutput;
};

class GumboParsingHelpers
{
public:
	static GumboNode* findSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent = 0) noexcept;
	static QByteArray nodeText(const GumboNode* node) noexcept;
	static std::vector<GumboNode*> subNodes(const GumboNode* node, GumboTag tag) noexcept;
	static unsigned countChildren(const GumboNode* node, GumboTag tag) noexcept;
	static QByteArray cutAllTagsFromNode(const GumboNode* node) noexcept;
	static QByteArray identifyHtmlPageContentType(const QByteArray& htmlPage) noexcept;
	static QByteArray decodeHtmlPage(const QByteArray& htmlPage) noexcept;

private:
	static void cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept;
};

}
