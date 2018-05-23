#pragma once

#include "ihtml_node.h"

namespace CrawlerEngine
{

class ResponseHeaders;
struct LinkInfo;

class IHtmlParser
{
public:
	virtual ~IHtmlParser() = default;

	virtual void parseHtmlPage(const QByteArray& htmlPage) = 0;
	virtual QByteArray identifyHtmlPageContentType() const = 0;
	virtual QByteArray decodeHtmlPage(const ResponseHeaders& headers) const = 0;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const = 0;
	virtual IHtmlNodeSharedPtr firstMatchNode(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodes(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeSharedPtr> matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const = 0;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const = 0;
	virtual IHtmlNodeSharedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const = 0;
};

}