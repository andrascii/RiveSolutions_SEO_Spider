#include "myhtml_parser.h"
#include "myhtml/api.h"
#include "response_headers.h"
#include "parsed_page.h"
#include "page_parser_helpers.h"
#include "finally.h"

namespace
{

const QByteArray s_aTagName = "a";
const QByteArray s_linkTagName = "link";
const QByteArray s_hrefAttributeName = "href";
const QByteArray s_relAttributeName = "rel";
const QByteArray s_relAttributeNofollowValue = "nofollow";

}

namespace CrawlerEngine
{

MyHtmlParser::MyHtmlParser()
	: m_myHtml(myhtml_create())
	, m_tree(myhtml_tree_create())
	, m_rootNode(nullptr)
	, m_regExp("[\\n\\t]+")
{
	myhtml_init(m_myHtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
	myhtml_tree_init(m_tree, m_myHtml);
}

MyHtmlParser::~MyHtmlParser()
{
	myhtml_tree_destroy(m_tree);
	myhtml_destroy(m_myHtml);
}

QByteArray MyHtmlParser::htmlPageContent() const
{
	return QByteArray();
}

void MyHtmlParser::parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers)
{
	headers;

	myhtml_parse(m_tree, MyENCODING_UTF_8, htmlPage.data(), htmlPage.size());
	myhtml_collection_t* collection = myhtml_get_nodes_by_tag_id(m_tree, nullptr, MyHTML_TAG_HTML, nullptr);

	if (!collection || !collection->list || !collection->length)
	{
		return;
	}

	for (std::size_t i = 0; i < collection->length; ++i)
	{
		if (myhtml_node_tag_id(collection->list[i]) != MyHTML_TAG_HTML)
		{
			continue;
		}

		m_rootNode = MyHtmlNode(collection->list[i]);
		break;
	}
}

QByteArray MyHtmlParser::decodeHtmlPage(const ResponseHeaders& headers)
{
	headers;
	return QByteArray();
}

std::vector<LinkInfo> MyHtmlParser::pageUrlList(bool httpOrHttpsOnly) const
{
	myhtml_collection_t* collection = myhtml_get_nodes_by_name(m_tree, nullptr, s_aTagName.data(), s_aTagName.size(), nullptr);

	if (!collection)
	{
		return std::vector<LinkInfo>();
	}

	Common::Finally destroyCollection([collection]
	{
		myhtml_collection_destroy(collection);
	});

	std::vector<LinkInfo> result;
	result.reserve(collection->length);

	for (size_t i = 0; i < collection->length; ++i)
	{
		myhtml_tree_attr_t* hrefAttribute = myhtml_attribute_by_key(collection->list[i], s_hrefAttributeName.data(), s_hrefAttributeName.size());

		if (!hrefAttribute)
		{
			continue;
		}

		const QString hrefAttributeValue = QString(myhtml_attribute_value(hrefAttribute, nullptr)).trimmed().remove(m_regExp);
		const Url url = hrefAttributeValue;

		if (httpOrHttpsOnly && !PageParserHelpers::isHttpOrHttpsScheme(Url(hrefAttributeValue)))
		{
			continue;
		}

		LinkParameter linkParam = LinkParameter::DofollowParameter;
		myhtml_tree_attr_t* relAttribute = myhtml_attribute_by_key(collection->list[i], s_relAttributeName.data(), s_relAttributeName.size());

		if (relAttribute)
		{
			QByteArray relAttrValue = QByteArray(myhtml_attribute_value(relAttribute, nullptr)).trimmed();

			if (relAttrValue == s_relAttributeNofollowValue)
			{
				linkParam = LinkParameter::NofollowParameter;
			}
		}

		const QString altOrTitle(myhtml_node_text(collection->list[i], nullptr));
		const bool dataResource = hrefAttributeValue.startsWith("data:");
		result.push_back(LinkInfo{ url, linkParam, altOrTitle, dataResource, ResourceSource::SourceTagA });
	}

	const LinkInfo canonical = getLinkRelUrl("canonical", ResourceSource::SourceTagLinkRelCanonical);

	if (canonical.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(canonical);
	}

	const LinkInfo next = getLinkRelUrl("next", ResourceSource::SourceTagLinkRelNext);

	if (next.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(next);
	}

	const LinkInfo prev = getLinkRelUrl("prev", ResourceSource::SourceTagLinkRelPrev);

	if (prev.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(prev);
	}

	return result;
}

IHtmlNodeSharedPtr MyHtmlParser::firstMatchNode(IHtmlNode::TagId tagId) const
{
	return m_rootNode.firstMatchSubNode(tagId);
}

std::vector<IHtmlNodeSharedPtr> MyHtmlParser::matchNodes(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodes(tagId);
}

std::vector<IHtmlNodeSharedPtr> MyHtmlParser::matchNodesInDepth(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodesInDepth(tagId);
}

std::vector<IHtmlNodeSharedPtr> MyHtmlParser::matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const
{
	return m_rootNode.matchSubNodesInDepth(predicate);
}

IHtmlNodeSharedPtr MyHtmlParser::findNodeWithAttributeValue(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const
{
	ASSERT(m_rootNode);

	return m_rootNode.childNodeByAttributeValue(tagId, expectedAttributes);
}

IHtmlNodeSharedPtr MyHtmlParser::findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const
{
	ASSERT(m_rootNode);

	return m_rootNode.childNodeByAttributesValues(tagId, expectedAttributes);
}

LinkInfo MyHtmlParser::getLinkRelUrl(const char* relValue, ResourceSource source) const
{
	myhtml_collection_t* collection = myhtml_get_nodes_by_name(m_tree, nullptr, s_linkTagName.data(), s_linkTagName.size(), nullptr);

	if (!collection)
	{
		return LinkInfo{ Url(), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceInvalid };
	}

	Common::Finally destroyCollection([collection]
	{
		myhtml_collection_destroy(collection);
	});

	for (size_t i = 0; i < collection->length; ++i)
	{
		myhtml_tree_attr_t* hrefAttribute = myhtml_attribute_by_key(collection->list[i], s_hrefAttributeName.data(), s_hrefAttributeName.size());
		myhtml_tree_attr_t* relAttribute = myhtml_attribute_by_key(collection->list[i], s_relAttributeName.data(), s_relAttributeName.size());

		if (!hrefAttribute || !relAttribute)
		{
			continue;
		}

		const QString relAttributeValue = QString(myhtml_attribute_value(relAttribute, nullptr)).trimmed().remove(m_regExp);

		if (relAttributeValue != relValue)
		{
			continue;
		}

		const QString hrefAttributeValue = QString(myhtml_attribute_value(hrefAttribute, nullptr)).trimmed().remove(m_regExp);
		const Url url = hrefAttributeValue;

		const QString altOrTitle(myhtml_node_text(collection->list[i], nullptr));
		const bool dataResource = hrefAttributeValue.startsWith("data:");
		
		return LinkInfo{ url, LinkParameter::DofollowParameter, altOrTitle, dataResource, source };
	}

	return LinkInfo{ Url(), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceInvalid };
}

}