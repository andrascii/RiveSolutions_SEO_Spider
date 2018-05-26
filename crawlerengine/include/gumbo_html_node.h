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
	virtual HtmlNodeSharedPtr findSubNode(TagId tagId, unsigned startIndexWhithinParent = 0) const override;
	virtual std::vector<HtmlNodeSharedPtr> subNodes(TagId tagId) const override;

private:
	GumboNode* m_node;
};

}