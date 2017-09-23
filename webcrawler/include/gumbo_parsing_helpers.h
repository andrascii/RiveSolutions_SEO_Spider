#pragma once

#include "gumbo.h"
#include "parsed_page.h"

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

	static std::vector<LinkInfo> parsePageUrlList(const GumboNode* node, bool httpOrHttpsOnly = true) noexcept;

	static const GumboNode* findChildNode(const GumboNode* node, GumboTag expectedTag, std::pair<const char*, const char*> expectedAttributes) noexcept;
	static const GumboNode* findChildNode(const GumboNode* node, GumboTag expectedTag, std::map<const char*, const char*> expectedAttributes) noexcept;

	static bool checkAttribute(const GumboNode* node, const char* attribute, const char* expectedValue) noexcept;


	template <class UnaryPredicate>
	static std::vector<const GumboNode*> findNodesRecursive(const GumboNode* node, UnaryPredicate predicate) noexcept
	{
		std::vector<const GumboNode*> result;

		if (predicate(node))
		{
			result.push_back(node);
		}

		if (node->type == GUMBO_NODE_ELEMENT)
		{
			const GumboVector* children = &node->v.element.children;

			for (unsigned int i = 0; i < children->length; ++i)
			{
				const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);
				std::vector<const GumboNode*> childResult = findNodesRecursive(child, predicate);

				result.insert(std::end(result), std::begin(childResult), std::end(childResult));
			}
		}

		return result;
	}

	template <class UnaryPredicate, class MapFunc>
	static auto findNodesAndGetResult(const GumboNode* node, UnaryPredicate predicate, MapFunc mapFunc)
	{
		std::vector<const GumboNode*> nodes = findNodesRecursive(node, predicate);

		std::vector<decltype(mapFunc(node))> result;
		for (const GumboNode* node : nodes)
		{
			result.push_back(mapFunc(node));
		}

		return result;
	}

private:
	static void cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept;
};

}
