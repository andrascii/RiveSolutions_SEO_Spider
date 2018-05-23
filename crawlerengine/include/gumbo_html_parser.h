#pragma once

#include "gumbo.h"
#include "ihtml_parser.h"
#include "gumbo_html_node.h"

namespace CrawlerEngine
{

enum class ResourceSource;

class GumboHtmlParser final : public IHtmlParser
{
public:
	GumboHtmlParser();
	GumboHtmlParser(const GumboOptions* options, const QByteArray& htmlPage);
	virtual ~GumboHtmlParser();

	virtual void parseHtmlPage(const QByteArray& htmlPage) override;
	virtual QByteArray identifyHtmlPageContentType() const override;
	virtual QByteArray decodeHtmlPage(const ResponseHeaders& headers) const override;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const override;
	virtual IHtmlNodeSharedPtr firstMatchNode(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodes(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(IHtmlNode::TagId tagId) const override;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const override;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const override;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const override;

private:
	LinkInfo getLinkRelUrl(const GumboNode* node, const char* relValue, ResourceSource source) const;

private:
	const GumboOptions* m_gumboOptions;
	GumboOutput* m_gumboOutput;
	QByteArray m_htmlPage;
	GumboHtmlNode m_rootNode;
};

}