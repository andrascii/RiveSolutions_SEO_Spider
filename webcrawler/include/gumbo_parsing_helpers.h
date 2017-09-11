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
	static const char* nodeText(const GumboNode* node) noexcept;
	static std::vector<GumboNode*> subNodes(const GumboNode* node, GumboTag tag) noexcept;
	static unsigned countChildren(const GumboNode* node, GumboTag tag) noexcept;
	static QByteArray cutAllTagsFromNode(const GumboNode* node) noexcept;
	static QByteArray identifyHtmlPageContentType(const QByteArray& htmlPage) noexcept;
	static QByteArray decodeHtmlPage(const QByteArray& htmlPage) noexcept;

	static std::vector<QUrl> parsePageUrlList(const GumboNode* node) noexcept;

	using ConditionFunc = bool(const GumboNode*);
	static std::vector<const GumboNode*> findNodesRecursive(const GumboNode* node, ConditionFunc) noexcept;

	static const GumboNode* findChildNode(const GumboNode* node, GumboTag expectedTag, std::pair<const char*, const char*> expectedAttributes) noexcept;
	static const GumboNode* findChildNode(const GumboNode* node, GumboTag expectedTag, std::map<const char*, const char*> expectedAttributes) noexcept;

	static bool checkAttribute(const GumboNode* node, const char* attribute, const char* expectedValue) noexcept;

	template <class TResultFunc>
	static auto findNodesAndGetResult(const GumboNode* node, ConditionFunc cond, TResultFunc res)
	{
		std::vector<const GumboNode*> nodes = findNodesRecursive(node, cond);

		std::vector<decltype(res(node))> result;
		for (const GumboNode* node : nodes)
		{
			result.push_back(res(node));
		}

		return result;
	}

private:
	static void cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept;
};

}
