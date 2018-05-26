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
	{ "UTF-8", MyENCODING_UTF_8 },
	{ "csUTF8", MyENCODING_UTF_8 },
	{ "UTF-16LE", MyENCODING_UTF_16LE },
	{ "csUTF16LE", MyENCODING_UTF_16LE },
	{ "UTF-16BE", MyENCODING_UTF_16BE },
	{ "csUTF16BE", MyENCODING_UTF_16BE },
	{ "Big5", MyENCODING_BIG5 },
	{ "csBig5", MyENCODING_BIG5 },
	{ "EUC-JP", MyENCODING_EUC_JP },
	{ "csEUCPkdFmtJapanese", MyENCODING_EUC_JP },
	{ "EUC-KR", MyENCODING_EUC_KR },
	{ "csEUCKR", MyENCODING_EUC_KR },
	{ "GB18030", MyENCODING_GB18030 },
	{ "csGB18030", MyENCODING_GB18030 },
	{ "GBK", MyENCODING_GBK },
	{ "CP936", MyENCODING_GBK },
	{ "MS936", MyENCODING_GBK },
	{ "windows-936", MyENCODING_GBK },
	{ "csGBK", MyENCODING_GBK },
	{ "IBM866", MyENCODING_IBM866 },
	{ "cp866", MyENCODING_IBM866 },
	{ "866", MyENCODING_IBM866 },
	{ "csIBM866", MyENCODING_IBM866 },
	{ "ISO-2022-JP", MyENCODING_ISO_2022_JP },
	{ "csISO2022JP", MyENCODING_ISO_2022_JP },
	{ "ISO-8859-10", MyENCODING_ISO_8859_10 },
	{ "iso-ir-157", MyENCODING_ISO_8859_10 },
	{ "l6", MyENCODING_ISO_8859_10 },
	{ "ISO_8859-10:1992", MyENCODING_ISO_8859_10 },
	{ "csISOLatin6", MyENCODING_ISO_8859_10 },
	{ "latin6", MyENCODING_ISO_8859_10 },
	{ "ISO-8859-13", MyENCODING_ISO_8859_13 },
	{ "csISO885913", MyENCODING_ISO_8859_13 },
	{ "ISO-8859-14", MyENCODING_ISO_8859_14 },
	{ "iso-ir-199", MyENCODING_ISO_8859_14 },
	{ "ISO_8859-14:1998", MyENCODING_ISO_8859_14 },
	{ "ISO_8859-14", MyENCODING_ISO_8859_14 },
	{ "latin8", MyENCODING_ISO_8859_14 },
	{ "iso-celtic", MyENCODING_ISO_8859_14 },
	{ "l8", MyENCODING_ISO_8859_14 },
	{ "csISO885914", MyENCODING_ISO_8859_14 },
	{ "ISO_8859-15", MyENCODING_ISO_8859_15 },
	{ "Latin-9", MyENCODING_ISO_8859_15 },
	{ "csISO885915", MyENCODING_ISO_8859_15 },
	{ "ISO-8859-16", MyENCODING_ISO_8859_16 },
	{ "iso-ir-226", MyENCODING_ISO_8859_16 },
	{ "ISO_8859-16:2001", MyENCODING_ISO_8859_16 },
	{ "ISO_8859-16", MyENCODING_ISO_8859_16 },
	{ "latin10", MyENCODING_ISO_8859_16 },
	{ "l10", MyENCODING_ISO_8859_16 },
	{ "csISO885916", MyENCODING_ISO_8859_16 },
	{ "ISO-8859-2", MyENCODING_ISO_8859_2 },
	{ "iso-ir-101", MyENCODING_ISO_8859_2 },
	{ "ISO_8859-2", MyENCODING_ISO_8859_2 },
	{ "latin2", MyENCODING_ISO_8859_2 },
	{ "l2", MyENCODING_ISO_8859_2 },
	{ "csISOLatin2", MyENCODING_ISO_8859_2 },
	{ "ISO-8859-3", MyENCODING_ISO_8859_3 },
	{ "iso-ir-109", MyENCODING_ISO_8859_3 },
	{ "ISO_8859-3", MyENCODING_ISO_8859_3 },
	{ "latin3", MyENCODING_ISO_8859_3 },
	{ "l3", MyENCODING_ISO_8859_3 },
	{ "csISOLatin3", MyENCODING_ISO_8859_3 },
	{ "ISO-8859-4", MyENCODING_ISO_8859_4 },
	{ "iso-ir-110", MyENCODING_ISO_8859_4 },
	{ "ISO_8859-4", MyENCODING_ISO_8859_4 },
	{ "latin4", MyENCODING_ISO_8859_4 },
	{ "l4", MyENCODING_ISO_8859_4 },
	{ "csISOLatin4", MyENCODING_ISO_8859_4 },
	{ "ISO-8859-5", MyENCODING_ISO_8859_5 },
	{ "iso-ir-144", MyENCODING_ISO_8859_5 },
	{ "ISO_8859-5", MyENCODING_ISO_8859_5 },
	{ "cyrillic", MyENCODING_ISO_8859_5 },
	{ "csISOLatinCyrillic", MyENCODING_ISO_8859_5 },
	{ "ISO-8859-6", MyENCODING_ISO_8859_6 },
	{ "iso-ir-127", MyENCODING_ISO_8859_6 },
	{ "ISO_8859-6", MyENCODING_ISO_8859_6 },
	{ "ECMA-114", MyENCODING_ISO_8859_6 },
	{ "ASMO-708", MyENCODING_ISO_8859_6 },
	{ "arabic", MyENCODING_ISO_8859_6 },
	{ "csISOLatinArabic", MyENCODING_ISO_8859_6 },
	{ "ISO-8859-7", MyENCODING_ISO_8859_7 },
	{ "iso-ir-126", MyENCODING_ISO_8859_7 },
	{ "ISO_8859-7", MyENCODING_ISO_8859_7 },
	{ "ELOT_928", MyENCODING_ISO_8859_7 },
	{ "ECMA-118", MyENCODING_ISO_8859_7 },
	{ "greek", MyENCODING_ISO_8859_7 },
	{ "greek8", MyENCODING_ISO_8859_7 },
	{ "csISOLatinGreek", MyENCODING_ISO_8859_7 },
	{ "ISO-8859-8", MyENCODING_ISO_8859_8 },
	{ "iso-ir-138", MyENCODING_ISO_8859_8 },
	{ "ISO_8859-8", MyENCODING_ISO_8859_8 },
	{ "hebrew", MyENCODING_ISO_8859_8 },
	{ "csISOLatinHebrew", MyENCODING_ISO_8859_8 },
	{ "ISO-8859-8-I", MyENCODING_ISO_8859_8_I },
	{ "csISO88598I", MyENCODING_ISO_8859_8_I },
	{ "KOI8-R", MyENCODING_KOI8_R },
	{ "csKOI8R", MyENCODING_KOI8_R },
	{ "KOI8-U", MyENCODING_KOI8_U },
	{ "csKOI8U", MyENCODING_KOI8_U },
	{ "macintosh", MyENCODING_MACINTOSH },
	{ "mac", MyENCODING_MACINTOSH },
	{ "csMacintosh", MyENCODING_MACINTOSH },
	{ "Shift_JIS", MyENCODING_SHIFT_JIS },
	{ "MS_Kanji", MyENCODING_SHIFT_JIS },
	{ "csShiftJIS", MyENCODING_SHIFT_JIS },
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
	{ "csISO111ECMACyrillic", MyENCODING_X_MAC_CYRILLIC }
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

void MyHtmlParser::parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers)
{
	m_htmlPage = htmlPage;

	myhtml_parse_fragment(m_tree, MyENCODING_UTF_8, htmlPage.data(), htmlPage.size(), MyHTML_TAG_HEAD, MyHTML_NAMESPACE_HTML);

	initRootNode();

	myencoding_t encoding = htmlSetEncoding(headers);
	myhtml_parse(m_tree, encoding, htmlPage.data(), htmlPage.size());
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

		const QString altOrTitle(MyHtmlNode(collection->list[i]).text());
		const bool dataResource = hrefAttributeValue.startsWith("data:");
		
		return LinkInfo{ url, LinkParameter::DofollowParameter, altOrTitle, dataResource, source };
	}

	return LinkInfo{ Url(), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceInvalid };
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

			myencoding_t encoding = s_stringToEnumEncoding.value(charsetFromHttpResponse.toLatin1(), static_cast<myencoding_t>(-1));

			if (encoding != -1)
			{
				myhtml_encoding_set(m_tree, encoding);
				return encoding;
			}
		}
	}

	const auto contentTypeValueFromPage = htmlPageContentType();

	const auto charsetFromHtmlPage = contentTypeValueFromPage.right(
		contentTypeValueFromPage.size() - contentTypeValueFromPage.lastIndexOf("=") - 1
	);

	if (charsetFromHtmlPage.isEmpty())
	{
		return MyENCODING_UTF_8;
	}

	myencoding_t encoding = s_stringToEnumEncoding.value(charsetFromHtmlPage, static_cast<myencoding_t>(-1));

	if (encoding == -1)
	{
		return MyENCODING_UTF_8;
	}

	myhtml_encoding_set(m_tree, encoding);
	return encoding;
}

QByteArray MyHtmlParser::htmlPageContentType() const
{
	IHtmlNodeSharedPtr headNode = m_rootNode.firstMatchSubNode(IHtmlNode::TagIdHead);

	if (!headNode)
	{
		DEBUGLOG << "tag <head> not found";

		return QByteArray();
	}

	const auto metaTags = headNode->matchSubNodes(IHtmlNode::TagIdMeta);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		if (!metaTags[i]->hasAttribute("content"))
		{
			continue;
		}

		const QString contentAttribute = metaTags[i]->attribute("content");

		if (metaTags[i]->hasAttribute("http-equiv"))
		{
			const QString attributeValue = metaTags[i]->attribute("http-equiv").toLower();

			if (attributeValue == "content-type")
			{
				return contentAttribute.toLatin1();
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

}