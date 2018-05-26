#pragma once

#include "gumbo.h"
#include "ihtml_node.h"

namespace CrawlerEngine
{

class GumboHtmlNode final : public IHtmlNode
{
public:
	GumboHtmlNode(GumboNode* node);

	virtual TagId tagId() const override;
	virtual NodeType type() const override;
	virtual QByteArray text() const override;
	virtual QString attribute(const QByteArray& attributeName) const override;
	virtual bool hasAttribute(const QByteArray& attributeName) const override;
	virtual operator bool() const override;
	virtual IHtmlNodeSharedPtr firstMatchSubNode(TagId tagId, unsigned startIndexWhithinParent = 0) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchSubNodes(TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchSubNodesInDepth(TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchSubNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual std::vector<IHtmlNodeSharedPtr> children() const override;
	virtual QByteArray cutSubNodesAndGetPlainText() const override;
	IHtmlNodeSharedPtr childNodeByAttributeValue(TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	IHtmlNodeSharedPtr childNodeByAttributesValues(TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

private:
	void matchSubNodesInDepthHelper(std::vector<IHtmlNodeSharedPtr>& result, GumboNode* node, const std::function<bool(const IHtmlNode&)>& predicate) const;
	void cutAllTagsFromNodeHelper(GumboNode* node, QByteArray& result) const;

private:
	GumboNode* m_node;
};

}