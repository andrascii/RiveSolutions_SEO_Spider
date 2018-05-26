#pragma once

#include "ihtml_parser.h"
#include "myhtml/api.h"

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
	virtual QByteArray decodeHtmlPage(const ResponseHeaders& headers) override;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const override;
	virtual IHtmlNodeSharedPtr firstMatchNode(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodes(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

private:
	LinkInfo getLinkRelUrl(const char* relValue, ResourceSource source) const;

private:
	myhtml_t* m_myHtml;
	myhtml_tree_t* m_tree;
	QRegularExpression m_regExp;
};

}