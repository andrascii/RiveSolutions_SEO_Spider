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

const QMap<QByteArray, myencoding_list> s_stringToEnumEncoding =
{
	{ "utf-8", MyENCODING_UTF_8 },
	{ "csutf8", MyENCODING_UTF_8 },
	{ "utf-16le", MyENCODING_UTF_16LE },
	{ "csutf16le", MyENCODING_UTF_16LE },
	{ "utf-16be", MyENCODING_UTF_16BE },
	{ "csutf16be", MyENCODING_UTF_16BE },
	{ "big5", MyENCODING_BIG5 },
	{ "csbig5", MyENCODING_BIG5 },
	{ "euc-jp", MyENCODING_EUC_JP },
	{ "cseucpkdfmtjapanese", MyENCODING_EUC_JP },
	{ "euc-kr", MyENCODING_EUC_KR },
	{ "cseuckr", MyENCODING_EUC_KR },
	{ "gb18030", MyENCODING_GB18030 },
	{ "csgb18030", MyENCODING_GB18030 },
	{ "gbk", MyENCODING_GBK },
	{ "cp936", MyENCODING_GBK },
	{ "ms936", MyENCODING_GBK },
	{ "windows-936", MyENCODING_GBK },
	{ "csgbk", MyENCODING_GBK },
	{ "ibm866", MyENCODING_IBM866 },
	{ "cp866", MyENCODING_IBM866 },
	{ "866", MyENCODING_IBM866 },
	{ "csibm866", MyENCODING_IBM866 },
	{ "iso-2022-jp", MyENCODING_ISO_2022_JP },
	{ "csiso2022jp", MyENCODING_ISO_2022_JP },
	{ "iso-8859-10", MyENCODING_ISO_8859_10 },
	{ "iso-ir-157", MyENCODING_ISO_8859_10 },
	{ "l6", MyENCODING_ISO_8859_10 },
	{ "iso_8859-10:1992", MyENCODING_ISO_8859_10 },
	{ "csisolatin6", MyENCODING_ISO_8859_10 },
	{ "latin6", MyENCODING_ISO_8859_10 },
	{ "iso-8859-13", MyENCODING_ISO_8859_13 },
	{ "csiso885913", MyENCODING_ISO_8859_13 },
	{ "iso-8859-14", MyENCODING_ISO_8859_14 },
	{ "iso-ir-199", MyENCODING_ISO_8859_14 },
	{ "iso_8859-14:1998", MyENCODING_ISO_8859_14 },
	{ "iso_8859-14", MyENCODING_ISO_8859_14 },
	{ "latin8", MyENCODING_ISO_8859_14 },
	{ "iso-celtic", MyENCODING_ISO_8859_14 },
	{ "l8", MyENCODING_ISO_8859_14 },
	{ "csiso885914", MyENCODING_ISO_8859_14 },
	{ "iso_8859-15", MyENCODING_ISO_8859_15 },
	{ "latin-9", MyENCODING_ISO_8859_15 },
	{ "csiso885915", MyENCODING_ISO_8859_15 },
	{ "iso-8859-16", MyENCODING_ISO_8859_16 },
	{ "iso-ir-226", MyENCODING_ISO_8859_16 },
	{ "iso_8859-16:2001", MyENCODING_ISO_8859_16 },
	{ "iso_8859-16", MyENCODING_ISO_8859_16 },
	{ "latin10", MyENCODING_ISO_8859_16 },
	{ "l10", MyENCODING_ISO_8859_16 },
	{ "csiso885916", MyENCODING_ISO_8859_16 },
	{ "iso-8859-2", MyENCODING_ISO_8859_2 },
	{ "iso-ir-101", MyENCODING_ISO_8859_2 },
	{ "iso_8859-2", MyENCODING_ISO_8859_2 },
	{ "latin2", MyENCODING_ISO_8859_2 },
	{ "l2", MyENCODING_ISO_8859_2 },
	{ "csisolatin2", MyENCODING_ISO_8859_2 },
	{ "iso-8859-3", MyENCODING_ISO_8859_3 },
	{ "iso-ir-109", MyENCODING_ISO_8859_3 },
	{ "iso_8859-3", MyENCODING_ISO_8859_3 },
	{ "latin3", MyENCODING_ISO_8859_3 },
	{ "l3", MyENCODING_ISO_8859_3 },
	{ "csisolatin3", MyENCODING_ISO_8859_3 },
	{ "iso-8859-4", MyENCODING_ISO_8859_4 },
	{ "iso-ir-110", MyENCODING_ISO_8859_4 },
	{ "iso_8859-4", MyENCODING_ISO_8859_4 },
	{ "latin4", MyENCODING_ISO_8859_4 },
	{ "l4", MyENCODING_ISO_8859_4 },
	{ "csisolatin4", MyENCODING_ISO_8859_4 },
	{ "iso-8859-5", MyENCODING_ISO_8859_5 },
	{ "iso-ir-144", MyENCODING_ISO_8859_5 },
	{ "iso_8859-5", MyENCODING_ISO_8859_5 },
	{ "cyrillic", MyENCODING_ISO_8859_5 },
	{ "csisolatincyrillic", MyENCODING_ISO_8859_5 },
	{ "iso-8859-6", MyENCODING_ISO_8859_6 },
	{ "iso-ir-127", MyENCODING_ISO_8859_6 },
	{ "iso_8859-6", MyENCODING_ISO_8859_6 },
	{ "ecma-114", MyENCODING_ISO_8859_6 },
	{ "asmo-708", MyENCODING_ISO_8859_6 },
	{ "arabic", MyENCODING_ISO_8859_6 },
	{ "csisolatinarabic", MyENCODING_ISO_8859_6 },
	{ "iso-8859-7", MyENCODING_ISO_8859_7 },
	{ "iso-ir-126", MyENCODING_ISO_8859_7 },
	{ "iso_8859-7", MyENCODING_ISO_8859_7 },
	{ "elot_928", MyENCODING_ISO_8859_7 },
	{ "ecma-118", MyENCODING_ISO_8859_7 },
	{ "greek", MyENCODING_ISO_8859_7 },
	{ "greek8", MyENCODING_ISO_8859_7 },
	{ "csisolatingreek", MyENCODING_ISO_8859_7 },
	{ "iso-8859-8", MyENCODING_ISO_8859_8 },
	{ "iso-ir-138", MyENCODING_ISO_8859_8 },
	{ "iso_8859-8", MyENCODING_ISO_8859_8 },
	{ "hebrew", MyENCODING_ISO_8859_8 },
	{ "csisolatinhebrew", MyENCODING_ISO_8859_8 },
	{ "iso-8859-8-I", MyENCODING_ISO_8859_8_I },
	{ "csiso88598i", MyENCODING_ISO_8859_8_I },
	{ "koi8-r", MyENCODING_KOI8_R },
	{ "cskoi8r", MyENCODING_KOI8_R },
	{ "koi8-u", MyENCODING_KOI8_U },
	{ "cskoi8u", MyENCODING_KOI8_U },
	{ "macintosh", MyENCODING_MACINTOSH },
	{ "mac", MyENCODING_MACINTOSH },
	{ "csmacintosh", MyENCODING_MACINTOSH },
	{ "shift_jis", MyENCODING_SHIFT_JIS },
	{ "ms_kanji", MyENCODING_SHIFT_JIS },
	{ "csshiftjis", MyENCODING_SHIFT_JIS },
	{ "windows-1250", MyENCODING_WINDOWS_1250 },
	{ "cswindows1250", MyENCODING_WINDOWS_1250 },
	{ "windows-1251", MyENCODING_WINDOWS_1251 },
	{ "cswindows1251", MyENCODING_WINDOWS_1251 },
	{ "windows-1252", MyENCODING_WINDOWS_1252 },
	{ "cswindows1252", MyENCODING_WINDOWS_1252 },
	{ "windows-1253", MyENCODING_WINDOWS_1253 },
	{ "cswindows1253", MyENCODING_WINDOWS_1253 },
	{ "windows-1254", MyENCODING_WINDOWS_1254 },
	{ "cswindows1254", MyENCODING_WINDOWS_1254 },
	{ "windows-1255", MyENCODING_WINDOWS_1255 },
	{ "cswindows1255", MyENCODING_WINDOWS_1255 },
	{ "windows-1256", MyENCODING_WINDOWS_1256 },
	{ "cswindows1256", MyENCODING_WINDOWS_1256 },
	{ "windows-1257", MyENCODING_WINDOWS_1257 },
	{ "cswindows1257", MyENCODING_WINDOWS_1257 },
	{ "windows-1258", MyENCODING_WINDOWS_1258 },
	{ "cswindows1258", MyENCODING_WINDOWS_1258 },
	{ "windows-874", MyENCODING_WINDOWS_874 },
	{ "cswindows874", MyENCODING_WINDOWS_874 },
	{ "csiso111ecmacyrillic", MyENCODING_X_MAC_CYRILLIC }
};

}

namespace CrawlerEngine
{

MyHtmlParser::MyHtmlParser()
	: m_myHtml(myhtml_create())
	, m_tree(myhtml_tree_create())
	, m_rootNode(nullptr)
	, m_regExp("[\\n\\t]+")
	, m_quotesRegExp("\"")
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
	return m_htmlPage;
}

bool MyHtmlParser::isEmpty() const
{
	return m_htmlPage.isEmpty();
}

void MyHtmlParser::parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers)
{
	m_htmlPage = htmlPage;

	myhtml_parse(m_tree, MyENCODING_UTF_8, htmlPage.data(), htmlPage.size());
	initRootNode();

	const myencoding_t encoding = htmlSetEncoding(headers);
	myhtml_parse(m_tree, encoding, htmlPage.data(), htmlPage.size());
	initRootNode();

	m_currentPageEncoding = encodingString(encoding);
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

		const QString altOrTitle(MyHtmlNode(collection->list[i]).text());
		const bool dataResource = hrefAttributeValue.startsWith("data:");
		result.push_back(LinkInfo{ url, linkParam, altOrTitle, dataResource, ResourceSource::SourceTagA });
	}

	const std::vector<LinkInfo> canonical = getLinkRelUrl("canonical", ResourceSource::SourceTagLinkRelCanonical);

	if (!canonical.empty())
	{
		result.push_back(canonical[0]);
	}

	const std::vector<LinkInfo> next = getLinkRelUrl("next", ResourceSource::SourceTagLinkRelNext);

	if (!next.empty())
	{
		result.push_back(next[0]);
	}

	const std::vector<LinkInfo> prev = getLinkRelUrl("prev", ResourceSource::SourceTagLinkRelPrev);

	if (!prev.empty())
	{
		result.push_back(prev[0]);
	}

	const std::vector<LinkInfo> hrefLang = getLinkRelUrl("alternate", ResourceSource::SourceTagLinkAlternateHrefLang, "hreflang", false);
	std::copy(hrefLang.cbegin(), hrefLang.cend(), std::back_inserter(result));

	return result;
}

IHtmlNodeCountedPtr MyHtmlParser::firstMatchNode(IHtmlNode::TagId tagId) const
{
	return m_rootNode.firstMatchSubNode(tagId);
}

std::vector<IHtmlNodeCountedPtr> MyHtmlParser::matchNodes(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodes(tagId);
}

std::vector<IHtmlNodeCountedPtr> MyHtmlParser::matchNodesInDepth(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodesInDepth(tagId);
}

std::vector<IHtmlNodeCountedPtr> MyHtmlParser::matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const
{
	return m_rootNode.matchSubNodesInDepth(predicate);
}

IHtmlNodeCountedPtr MyHtmlParser::findNodeWithAttributeValue(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const
{
	ASSERT(m_rootNode);

	return m_rootNode.childNodeByAttributeValue(tagId, expectedAttributes);
}

IHtmlNodeCountedPtr MyHtmlParser::findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const
{
	ASSERT(m_rootNode);

	return m_rootNode.childNodeByAttributesValues(tagId, expectedAttributes);
}

std::vector<LinkInfo> MyHtmlParser::getLinkRelUrl(const char* relValue, ResourceSource source, const char* requiredAttribute, bool getFirstValueOnly) const
{
	std::vector<LinkInfo> result;
	myhtml_collection_t* collection = myhtml_get_nodes_by_name(m_tree, nullptr, s_linkTagName.data(), s_linkTagName.size(), nullptr);

	if (!collection)
	{
		return result;
	}

	Common::Finally destroyCollection([collection]
	{
		myhtml_collection_destroy(collection);
	});

	for (size_t i = 0; i < collection->length; ++i)
	{
		myhtml_tree_attr_t* hrefAttribute = myhtml_attribute_by_key(collection->list[i], s_hrefAttributeName.data(), s_hrefAttributeName.size());
		myhtml_tree_attr_t* relAttribute = myhtml_attribute_by_key(collection->list[i], s_relAttributeName.data(), s_relAttributeName.size());
		const bool hasRequiredAttribute = requiredAttribute == nullptr || myhtml_attribute_by_key(collection->list[i], requiredAttribute, strlen(requiredAttribute));

		if (!hrefAttribute || !relAttribute || !hasRequiredAttribute)
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

		const QString altOrTitle(MyHtmlNode(collection->list[i]).text());
		const bool dataResource = hrefAttributeValue.startsWith("data:");
		
		result.push_back({ url, LinkParameter::DofollowParameter, altOrTitle, dataResource, source });
		if (getFirstValueOnly)
		{
			return result;
		}
	}

	return result;
}

myencoding_t MyHtmlParser::htmlSetEncoding(const ResponseHeaders& headers)
{
	const auto contentTypeValues = headers.valueOf("content-type");

	if (!contentTypeValues.empty())
	{
		for (const QString& contentTypeValue : contentTypeValues)
		{
			QString charsetFromHttpResponse = contentTypeValue.right(contentTypeValue.size() - contentTypeValue.lastIndexOf("=") - 1);
			charsetFromHttpResponse.remove(m_quotesRegExp);

			myencoding_t encoding = s_stringToEnumEncoding.value(charsetFromHttpResponse.toLatin1().toLower(), static_cast<myencoding_t>(-1));

			if (encoding != -1)
			{
				myhtml_encoding_set(m_tree, encoding);
				return encoding;
			}
		}
	}

	const auto charsetFromHtmlPage = encodingFromPage();

	if (charsetFromHtmlPage.isEmpty())
	{
		return MyENCODING_UTF_8;
	}

	myencoding_t encoding = s_stringToEnumEncoding.value(charsetFromHtmlPage.toLower(), static_cast<myencoding_t>(-1));

	if (encoding == -1)
	{
		return MyENCODING_UTF_8;
	}

	myhtml_encoding_set(m_tree, encoding);
	return encoding;
}

QByteArray MyHtmlParser::encodingFromPage() const
{
	IHtmlNodeCountedPtr headNode = m_rootNode.firstMatchSubNode(IHtmlNode::TagIdHead);

	if (!headNode)
	{
		INFOLOG << "tag <head> not found";
		return QByteArray();
	}

	const auto metaTags = headNode->matchSubNodes(IHtmlNode::TagIdMeta);

	for (const auto& metaTag : metaTags)
	{
		const bool hasContentAttribute = metaTag->hasAttribute("content");
		const bool hasCharsetAttribute = metaTag->hasAttribute("charset");

		if (!hasContentAttribute && !hasCharsetAttribute)
		{
			continue;
		}

		if (hasCharsetAttribute)
		{
			return metaTag->attribute("charset").toLatin1();
		}
		
		const QString contentAttribute = metaTag->attribute("content");

		if (metaTag->hasAttribute("http-equiv"))
		{
			const QString attributeValue = metaTag->attribute("http-equiv").toLower();

			if (attributeValue == "content-type")
			{
				const auto charsetFromHtmlPage = contentAttribute.right(
					contentAttribute.size() - contentAttribute.lastIndexOf("=") - 1
				);

				return charsetFromHtmlPage.toLatin1();
			}
		}
	}

	return QByteArray();
}

void MyHtmlParser::initRootNode()
{
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

QByteArray MyHtmlParser::currentPageEncoding() const
{
	return m_currentPageEncoding;
}

QByteArray MyHtmlParser::encodingString(myencoding_t encoding) const
{
	for (auto iter = s_stringToEnumEncoding.begin(); iter != s_stringToEnumEncoding.end(); ++iter)
	{
		if (iter.value() != encoding)
		{
			continue;
		}

		return iter.key();
	}

	return QByteArray();
}

}