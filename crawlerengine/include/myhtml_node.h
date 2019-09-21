#pragma once

#include "ihtml_node.h"
#include "myhtml/api.h"

namespace CrawlerEngine
{

class MyHtmlNode final : public IHtmlNode
{
public:
	MyHtmlNode(myhtml_tree_node_t* node);

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
	virtual IHtmlNodeCountedPtr firstMatchSubNode(TagId tagId, unsigned startIndexWhithinParent = 0) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodes(TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual std::vector<IHtmlNodeCountedPtr> children() const override;
	virtual QByteArray cutSubNodesAndGetPlainText() const override;
	virtual IHtmlNodeCountedPtr childNodeByAttributeValue(TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	virtual IHtmlNodeCountedPtr childNodeByAttributesValues(TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

	virtual void* data() const override;

private:
	void matchSubNodesInDepthHelper(myhtml_tree_node_t* node, 
		std::vector<IHtmlNodeCountedPtr>& collection, const std::function<bool(const IHtmlNode&)>& predicate) const;

	void cutAllTagsFromNodeHelper(myhtml_tree_node_t* node, QByteArray& result) const;

	std::vector<myhtml_tree_node_t*> childrenInternal(myhtml_tree_node_t* node) const;

private:
	myhtml_tree_node_t* m_node;
};

}