#pragma once

#include "ihtml_parser.h"
#include "myhtml/api.h"
#include "myhtml_node.h"

namespace CrawlerEngine
{

enum class ResourceSource;

class MyHtmlParser final : public IHtmlParser
{
public:
	MyHtmlParser();
	virtual ~MyHtmlParser();

	virtual QByteArray htmlPageContent() const override;
	virtual void parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers) override;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const override;
	virtual IHtmlNodeCountedPtr firstMatchNode(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodes(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodesInDepth(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual IHtmlNodeCountedPtr findNodeWithAttributeValue(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	virtual IHtmlNodeCountedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

private:
	LinkInfo getLinkRelUrl(const char* relValue, ResourceSource source) const;
	myencoding_t htmlSetEncoding(const ResponseHeaders& headers);
	QByteArray htmlPageContentType() const;
	void initRootNode();

private:
	myhtml_t* m_myHtml;
	myhtml_tree_t* m_tree;
	MyHtmlNode m_rootNode;
	QByteArray m_htmlPage;
	QRegularExpression m_regExp;
	QRegularExpression m_quotesRegExp;
};

}