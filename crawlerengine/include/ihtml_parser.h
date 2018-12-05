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

	virtual QByteArray currentPageEncoding() const = 0;
	virtual QByteArray htmlPageContent() const = 0;
	virtual bool isEmpty() const = 0;
	virtual void parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers) = 0;
	virtual std::vector<LinkInfo> pageUrlList(bool httpOrHttpsOnly) const = 0;
	virtual IHtmlNodeCountedPtr firstMatchNode(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodes(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodesInDepth(IHtmlNode::TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const = 0;
	virtual IHtmlNodeCountedPtr findNodeWithAttributeValue(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const = 0;
	virtual IHtmlNodeCountedPtr findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const = 0;
};

}