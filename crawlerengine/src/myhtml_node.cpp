#include "myhtml_node.h"

namespace
{

using namespace CrawlerEngine;

const QMap<std::size_t, IHtmlNode::TagId> s_tagIdMapping
{
	{ MyHTML_TAG_HTML,               IHtmlNode::TagIdHtml },
	{ MyHTML_TAG_HEAD,               IHtmlNode::TagIdHead },
	{ MyHTML_TAG_TITLE,              IHtmlNode::TagIdTitle },
	{ MyHTML_TAG_BASE,               IHtmlNode::TagIdBase },
	{ MyHTML_TAG_LINK,               IHtmlNode::TagIdLink },
	{ MyHTML_TAG_META,               IHtmlNode::TagIdMeta },
	{ MyHTML_TAG_STYLE,              IHtmlNode::TagIdStyle },
	{ MyHTML_TAG_SCRIPT,             IHtmlNode::TagIdScript },
	{ MyHTML_TAG_NOSCRIPT,           IHtmlNode::TagIdNoScript },
	{ MyHTML_TAG_TEMPLATE,           IHtmlNode::TagIdTemplate },
	{ MyHTML_TAG_BODY,               IHtmlNode::TagIdBody },
	{ MyHTML_TAG_ARTICLE,            IHtmlNode::TagIdArticle },
	{ MyHTML_TAG_SECTION,            IHtmlNode::TagIdSection },
	{ MyHTML_TAG_NAV,                IHtmlNode::TagIdNav },
	{ MyHTML_TAG_ASIDE,              IHtmlNode::TagIdAside },
	{ MyHTML_TAG_H1,                 IHtmlNode::TagIdH1 },
	{ MyHTML_TAG_H2,                 IHtmlNode::TagIdH2 },
	{ MyHTML_TAG_H3,                 IHtmlNode::TagIdH3 },
	{ MyHTML_TAG_H4,                 IHtmlNode::TagIdH4 },
	{ MyHTML_TAG_H5,                 IHtmlNode::TagIdH5 },
	{ MyHTML_TAG_H6,                 IHtmlNode::TagIdH6 },
	{ MyHTML_TAG_HGROUP,             IHtmlNode::TagIdHGroup },
	{ MyHTML_TAG_HEADER,             IHtmlNode::TagIdHeader },
	{ MyHTML_TAG_FOOTER,             IHtmlNode::TagIdFooter },
	{ MyHTML_TAG_ADDRESS,            IHtmlNode::TagIdAddress },
	{ MyHTML_TAG_P,                  IHtmlNode::TagIdP },
	{ MyHTML_TAG_HR,                 IHtmlNode::TagIdHr },
	{ MyHTML_TAG_PRE,                IHtmlNode::TagIdPre },
	{ MyHTML_TAG_BLOCKQUOTE,         IHtmlNode::TagIdBlockQuote },
	{ MyHTML_TAG_OL,                 IHtmlNode::TagIdOl },
	{ MyHTML_TAG_UL,                 IHtmlNode::TagIdUl },
	{ MyHTML_TAG_LI,                 IHtmlNode::TagIdLi },
	{ MyHTML_TAG_DL,                 IHtmlNode::TagIdDl },
	{ MyHTML_TAG_DT,                 IHtmlNode::TagIdDt },
	{ MyHTML_TAG_DD,                 IHtmlNode::TagIdDd },
	{ MyHTML_TAG_FIGURE,             IHtmlNode::TagIdFigure },
	{ MyHTML_TAG_FIGCAPTION,         IHtmlNode::TagIdFigCaption },
	{ MyHTML_TAG_MAIN,               IHtmlNode::TagIdMain },
	{ MyHTML_TAG_DIV,                IHtmlNode::TagIdDiv },
	{ MyHTML_TAG_A,                  IHtmlNode::TagIdA },
	{ MyHTML_TAG_EM,                 IHtmlNode::TagIdEm },
	{ MyHTML_TAG_STRONG,             IHtmlNode::TagIdStrong },
	{ MyHTML_TAG_SMALL,              IHtmlNode::TagIdSmall },
	{ MyHTML_TAG_S,                  IHtmlNode::TagIdS },
	{ MyHTML_TAG_CITE,               IHtmlNode::TagIdCite },
	{ MyHTML_TAG_Q,                  IHtmlNode::TagIdQ },
	{ MyHTML_TAG_DFN,                IHtmlNode::TagIdDfn },
	{ MyHTML_TAG_ABBR,               IHtmlNode::TagIdAbbr },
	//{ MyHTML_TAG_DATA,               IHtmlNode::TagIdData },
	{ MyHTML_TAG_TIME,               IHtmlNode::TagIdTime },
	{ MyHTML_TAG_CODE,               IHtmlNode::TagIdCode },
	{ MyHTML_TAG_VAR,                IHtmlNode::TagIdVar },
	{ MyHTML_TAG_SAMP,               IHtmlNode::TagIdSamp },
	{ MyHTML_TAG_KBD,                IHtmlNode::TagIdKbd },
	{ MyHTML_TAG_SUB,                IHtmlNode::TagIdSub },
	{ MyHTML_TAG_SUP,                IHtmlNode::TagIdSup },
	{ MyHTML_TAG_I,                  IHtmlNode::TagIdI },
	{ MyHTML_TAG_B,                  IHtmlNode::TagIdB },
	{ MyHTML_TAG_U,                  IHtmlNode::TagIdU },
	{ MyHTML_TAG_MARK,               IHtmlNode::TagIdMark },
	{ MyHTML_TAG_RUBY,               IHtmlNode::TagIdRuby },
	{ MyHTML_TAG_RT,                 IHtmlNode::TagIdRt },
	{ MyHTML_TAG_RP,                 IHtmlNode::TagIdRp },
	{ MyHTML_TAG_BDI,                IHtmlNode::TagIdBdi },
	{ MyHTML_TAG_BDO,                IHtmlNode::TagIdBdo },
	{ MyHTML_TAG_SPAN,               IHtmlNode::TagIdSpan },
	{ MyHTML_TAG_BR,                 IHtmlNode::TagIdBr },
	{ MyHTML_TAG_WBR,                IHtmlNode::TagIdWbr },
	{ MyHTML_TAG_INS,                IHtmlNode::TagIdIns },
	{ MyHTML_TAG_DEL,                IHtmlNode::TagIdDel },
	{ MyHTML_TAG_IMAGE,              IHtmlNode::TagIdImage },
	{ MyHTML_TAG_IMG,                IHtmlNode::TagIdImg },
	{ MyHTML_TAG_IFRAME,             IHtmlNode::TagIdIframe },
	{ MyHTML_TAG_EMBED,              IHtmlNode::TagIdEmbed },
	{ MyHTML_TAG_OBJECT,             IHtmlNode::TagIdObject },
	{ MyHTML_TAG_PARAM,              IHtmlNode::TagIdParam },
	{ MyHTML_TAG_VIDEO,              IHtmlNode::TagIdVideo },
	{ MyHTML_TAG_AUDIO,              IHtmlNode::TagIdAudio },
	{ MyHTML_TAG_SOURCE,             IHtmlNode::TagIdSource },
	{ MyHTML_TAG_TRACK,              IHtmlNode::TagIdTrack },
	{ MyHTML_TAG_CANVAS,             IHtmlNode::TagIdCanvas },
	{ MyHTML_TAG_MAP,                IHtmlNode::TagIdMap },
	{ MyHTML_TAG_AREA,               IHtmlNode::TagIdArea },
	{ MyHTML_TAG_MATH,               IHtmlNode::TagIdMath },
	{ MyHTML_TAG_MI,                 IHtmlNode::TagIdMi },
	{ MyHTML_TAG_MO,                 IHtmlNode::TagIdMo },
	{ MyHTML_TAG_MN,                 IHtmlNode::TagIdMn },
	{ MyHTML_TAG_MS,                 IHtmlNode::TagIdMs },
	{ MyHTML_TAG_MTEXT,              IHtmlNode::TagIdMtext },
	{ MyHTML_TAG_MGLYPH,             IHtmlNode::TagIdMglyph },
	{ MyHTML_TAG_MALIGNMARK,         IHtmlNode::TagIdMalignMark },
	{ MyHTML_TAG_ANNOTATION_XML,     IHtmlNode::TagIdAnnotationXml },
	{ MyHTML_TAG_SVG,                IHtmlNode::TagIdSvg },
	{ MyHTML_TAG_FOREIGNOBJECT,      IHtmlNode::TagIdForeignObject },
	{ MyHTML_TAG_DESC,               IHtmlNode::TagIdDesc },
	{ MyHTML_TAG_TABLE,              IHtmlNode::TagIdTable },
	{ MyHTML_TAG_CAPTION,            IHtmlNode::TagIdCaption },
	{ MyHTML_TAG_COLGROUP,           IHtmlNode::TagIdColGroup },
	{ MyHTML_TAG_COL,                IHtmlNode::TagIdCol },
	{ MyHTML_TAG_TBODY,              IHtmlNode::TagIdTbody },
	{ MyHTML_TAG_THEAD,              IHtmlNode::TagIdThead },
	{ MyHTML_TAG_TFOOT,              IHtmlNode::TagIdTfoot },
	{ MyHTML_TAG_TR,                 IHtmlNode::TagIdTr },
	{ MyHTML_TAG_TD,                 IHtmlNode::TagIdTd },
	{ MyHTML_TAG_TH,                 IHtmlNode::TagIdTh },
	{ MyHTML_TAG_FORM,               IHtmlNode::TagIdForm },
	{ MyHTML_TAG_FIELDSET,           IHtmlNode::TagIdFieldSet },
	{ MyHTML_TAG_LEGEND,             IHtmlNode::TagIdLegend },
	{ MyHTML_TAG_LABEL,              IHtmlNode::TagIdLabel },
	{ MyHTML_TAG_INPUT,              IHtmlNode::TagIdInput },
	{ MyHTML_TAG_BUTTON,             IHtmlNode::TagIdButton },
	{ MyHTML_TAG_SELECT,             IHtmlNode::TagIdSelect },
	{ MyHTML_TAG_DATALIST,           IHtmlNode::TagIdDataList },
	{ MyHTML_TAG_OPTGROUP,           IHtmlNode::TagIdOptGroup },
	{ MyHTML_TAG_OPTION,             IHtmlNode::TagIdOption },
	{ MyHTML_TAG_TEXTAREA,           IHtmlNode::TagIdTextArea },
	{ MyHTML_TAG_KEYGEN,             IHtmlNode::TagIdKeygen },
	{ MyHTML_TAG_OUTPUT,             IHtmlNode::TagIdOutput },
	{ MyHTML_TAG_PROGRESS,           IHtmlNode::TagIdProgress },
	{ MyHTML_TAG_METER,              IHtmlNode::TagIdMeter },
	{ MyHTML_TAG_DETAILS,            IHtmlNode::TagIdDetails },
	{ MyHTML_TAG_SUMMARY,            IHtmlNode::TagIdSummary },
	{ MyHTML_TAG_MENU,               IHtmlNode::TagIdMenu },
	{ MyHTML_TAG_MENUITEM,           IHtmlNode::TagIdMenuItem },
	{ MyHTML_TAG_APPLET,             IHtmlNode::TagIdApplet },
	{ MyHTML_TAG_ACRONYM,            IHtmlNode::TagIdAcronym },
	{ MyHTML_TAG_BGSOUND,            IHtmlNode::TagIdBgSound },
	{ MyHTML_TAG_DIR,                IHtmlNode::TagIdDir },
	{ MyHTML_TAG_FRAME,              IHtmlNode::TagIdFrame },
	{ MyHTML_TAG_FRAMESET,           IHtmlNode::TagIdFrameSet },
	{ MyHTML_TAG_NOFRAMES,           IHtmlNode::TagIdNoFrames },
	{ MyHTML_TAG_ISINDEX,            IHtmlNode::TagIdIsIndex },
	{ MyHTML_TAG_LISTING,            IHtmlNode::TagIdListing },
	{ MyHTML_TAG_XMP,                IHtmlNode::TagIdXmp },
	//{ MyHTML_TAG_NEXTID,             IHtmlNode::TagIdNextId },
	{ MyHTML_TAG_NOEMBED,            IHtmlNode::TagIdNoEmbed },
	{ MyHTML_TAG_PLAINTEXT,          IHtmlNode::TagIdPlainText },
	{ MyHTML_TAG_RB,                 IHtmlNode::TagIdRb },
	{ MyHTML_TAG_STRIKE,             IHtmlNode::TagIdStrike },
	{ MyHTML_TAG_BASEFONT,           IHtmlNode::TagIdBaseFont },
	{ MyHTML_TAG_BIG,                IHtmlNode::TagIdBig },
	{ MyHTML_TAG_BLINK,              IHtmlNode::TagIdBlink },
	{ MyHTML_TAG_CENTER,             IHtmlNode::TagIdCenter },
	{ MyHTML_TAG_FONT,               IHtmlNode::TagIdFont },
	{ MyHTML_TAG_MARQUEE,            IHtmlNode::TagIdMarquee },
	//{ MyHTML_TAG_MULTICOL,           IHtmlNode::TagIdMulticol },
	{ MyHTML_TAG_NOBR,               IHtmlNode::TagIdNoBr },
	//{ MyHTML_TAG_SPACER,             IHtmlNode::TagIdSpacer },
	{ MyHTML_TAG_TT,                 IHtmlNode::TagIdTt },
	{ MyHTML_TAG_RTC,                IHtmlNode::TagIdRtc },
	//{ MyHTML_TAG_UNKNOWN,            IHtmlNode::TagIdUnknown }
};

}

namespace CrawlerEngine
{

MyHtmlNode::MyHtmlNode(myhtml_tree_node_t* node)
	: m_node(node)
{
}

IHtmlNode::TagId MyHtmlNode::tagId() const
{
	if (!m_node)
	{
		return IHtmlNode::TagIdUnknown;
	}

	return s_tagIdMapping[myhtml_node_tag_id(m_node)];
}

IHtmlNode::NodeType MyHtmlNode::type() const
{
	return IHtmlNode::NodeTypeElement;
}

QByteArray MyHtmlNode::text() const
{
	if (!m_node)
	{
		return QByteArray();
	}

	const auto childNodes = childrenInternal(m_node);

	for (auto* child : childNodes)
	{
		if (myhtml_node_tag_id(child) != MyHTML_TAG__TEXT)
		{
			continue;
		}

		return myhtml_node_text(child, nullptr);
	}

	return QByteArray();
}

QString MyHtmlNode::attribute(const QByteArray& attributeName) const
{
	myhtml_tree_attr_t* attribute = myhtml_attribute_by_key(m_node, attributeName.data(), attributeName.size());

	if (!attribute)
	{
		return QString();
	}

	return myhtml_attribute_value(attribute, nullptr);
}

bool MyHtmlNode::hasAttribute(const QByteArray& attributeName) const
{
	myhtml_tree_attr_t* attribute = myhtml_attribute_by_key(m_node, attributeName.data(), attributeName.size());

	return attribute != nullptr;
}

MyHtmlNode::operator bool() const
{
	return m_node != nullptr;
}

IHtmlNodeSharedPtr MyHtmlNode::firstMatchSubNode(TagId tagId, unsigned startIndexWhithinParent) const
{
	startIndexWhithinParent;

	myhtml_tree_node_t* node = myhtml_node_child(m_node);

	while (node)
	{
		myhtml_tag_id_t tagIdentifier = myhtml_node_tag_id(node);

		if (s_tagIdMapping[tagIdentifier] == tagId)
		{
			return std::make_shared<MyHtmlNode>(node);
		}

		node = myhtml_node_next(node);
	}

	return nullptr;
}

std::vector<IHtmlNodeSharedPtr> MyHtmlNode::matchSubNodes(TagId tagId) const
{
	myhtml_tree_node_t* node = m_node;
	std::vector<IHtmlNodeSharedPtr> result;

	while (node)
	{
		myhtml_tag_id_t tagIdentifier = myhtml_node_tag_id(node);

		if (s_tagIdMapping[tagIdentifier] == tagId)
		{
			result.push_back(std::make_shared<MyHtmlNode>(node));
		}

		node = myhtml_node_next(node);
	}

	return result;
}

std::vector<IHtmlNodeSharedPtr> MyHtmlNode::matchSubNodesInDepth(TagId tagId) const
{
	std::vector<IHtmlNodeSharedPtr> collection;

	matchSubNodesInDepthHelper(myhtml_node_child(m_node), collection, [tagId](const IHtmlNode& node)
	{
		return node.tagId() == tagId;
	});

	return collection;
}

std::vector<IHtmlNodeSharedPtr> MyHtmlNode::matchSubNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const
{
	std::vector<IHtmlNodeSharedPtr> collection;
	matchSubNodesInDepthHelper(myhtml_node_child(m_node), collection, predicate);
	return collection;
}

std::vector<IHtmlNodeSharedPtr> MyHtmlNode::children() const
{
	const auto internalChildrenRepresentation = childrenInternal(m_node);

	std::vector<IHtmlNodeSharedPtr> result;
	result.reserve(internalChildrenRepresentation.size());

	for (auto* child : internalChildrenRepresentation)
	{
		result.emplace_back(std::make_shared<MyHtmlNode>(child));
	}

	return result;
}

QByteArray MyHtmlNode::cutSubNodesAndGetPlainText() const
{
	QByteArray result;
	cutAllTagsFromNodeHelper(m_node, result);

	return result;
}

IHtmlNodeSharedPtr MyHtmlNode::childNodeByAttributeValue(TagId tagId, std::pair<const char*, const char*> expectedAttributes) const
{
	return childNodeByAttributesValues(tagId, std::map<const char*, const char*>{ expectedAttributes });
}

IHtmlNodeSharedPtr MyHtmlNode::childNodeByAttributesValues(TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const
{
	if (!m_node)
	{
		return nullptr;
	}

	const auto childNodes = childrenInternal(m_node);

	for (auto* child : childNodes)
	{
		if (s_tagIdMapping[myhtml_node_tag_id(child)] != tagId)
		{
			continue;
		}

		MyHtmlNode foundNode(child);
		bool success = true;

		for (const auto& [attributeName, attributeValue] : expectedAttributes)
		{
			success = success && foundNode.hasAttribute(attributeName) && 
				(strlen(attributeValue) == 0 || foundNode.attribute(attributeName).toLower() == attributeValue);

			if (!success)
			{
				break;
			}
		}

		if (success)
		{
			return std::make_shared<MyHtmlNode>(child);
		}
	}

	return nullptr;
}

void MyHtmlNode::matchSubNodesInDepthHelper(myhtml_tree_node_t* node, 
	std::vector<IHtmlNodeSharedPtr>& collection, const std::function<bool(const IHtmlNode&)>& predicate) const
{
	while (node)
	{
		if (predicate(MyHtmlNode(node)))
		{
			collection.push_back(std::make_shared<MyHtmlNode>(node));
		}

		myhtml_tree_node_t* child = myhtml_node_child(node);

		if (child)
		{
			matchSubNodesInDepthHelper(child, collection, predicate);
		}

		node = myhtml_node_next(node);
	}
}

void MyHtmlNode::cutAllTagsFromNodeHelper(myhtml_tree_node_t* node, QByteArray& result) const
{
	if (!node)
	{
		return;
	}

	myhtml_tag_id_t tagId = myhtml_node_tag_id(node);

	if (tagId == MyHTML_TAG__TEXT)
	{
		result += QByteArray(MyHtmlNode(node).text()) + " ";
		return;
	}

	const auto childNodes = childrenInternal(node);

	for (unsigned i = 0; i < childNodes.size(); ++i)
	{
		myhtml_tree_node_t* child = childNodes[i];
		cutAllTagsFromNodeHelper(child, result);
	}
}

std::vector<myhtml_tree_node_t*> MyHtmlNode::childrenInternal(myhtml_tree_node_t* node) const
{
	myhtml_tree_node_t* child = myhtml_node_child(node);
	std::vector<myhtml_tree_node_t*> result;

	while (child)
	{
		result.push_back(child);
		child = myhtml_node_next(child);
	}

	return result;
}

}