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
	virtual IHtmlNodeCountedPtr parent() const override;
	virtual IHtmlNodeCountedPtr firstChild() const override;
	virtual IHtmlNodeCountedPtr nextSibling() const override;
	virtual IHtmlNodeCountedPtr prevSibling() const override;
	virtual int childIndex() const override;
	virtual int attributesCount() const override;
	virtual IHtmlAttributeCountedPtr attribute(int index) const override;
	virtual IHtmlNodeCountedPtr firstMatchSubNode(TagId tagId, unsigned startIndexWhithinParent = 0) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodes(TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual std::vector<IHtmlNodeCountedPtr> children() const override;
	virtual QByteArray cutSubNodesAndGetPlainText() const override;
	IHtmlNodeCountedPtr childNodeByAttributeValue(TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	IHtmlNodeCountedPtr childNodeByAttributesValues(TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

	virtual void* data() const override;

private:
	void matchSubNodesInDepthHelper(std::vector<IHtmlNodeCountedPtr>& result, GumboNode* node, const std::function<bool(const IHtmlNode&)>& predicate) const;
	void cutAllTagsFromNodeHelper(GumboNode* node, QByteArray& result) const;

private:
	GumboNode* m_node;
};

}