#include "gumbo_html_node.h"

namespace
{

using namespace CrawlerEngine;

const QMap<int, IHtmlNode::TagId> s_tagIdMapping
{
	{ GUMBO_TAG_HTML,               IHtmlNode::TagIdHtml },
	{ GUMBO_TAG_HEAD,               IHtmlNode::TagIdHead },
	{ GUMBO_TAG_TITLE,              IHtmlNode::TagIdTitle },
	{ GUMBO_TAG_BASE,               IHtmlNode::TagIdBase },
	{ GUMBO_TAG_LINK,               IHtmlNode::TagIdLink },
	{ GUMBO_TAG_META,               IHtmlNode::TagIdMeta },
	{ GUMBO_TAG_STYLE,              IHtmlNode::TagIdStyle },
	{ GUMBO_TAG_SCRIPT,             IHtmlNode::TagIdScript },
	{ GUMBO_TAG_NOSCRIPT,           IHtmlNode::TagIdNoScript },
	{ GUMBO_TAG_TEMPLATE,           IHtmlNode::TagIdTemplate },
	{ GUMBO_TAG_BODY,               IHtmlNode::TagIdBody },
	{ GUMBO_TAG_ARTICLE,            IHtmlNode::TagIdArticle },
	{ GUMBO_TAG_SECTION,            IHtmlNode::TagIdSection },
	{ GUMBO_TAG_NAV,                IHtmlNode::TagIdNav },
	{ GUMBO_TAG_ASIDE,              IHtmlNode::TagIdAside },
	{ GUMBO_TAG_H1,                 IHtmlNode::TagIdH1 },
	{ GUMBO_TAG_H2,                 IHtmlNode::TagIdH2 },
	{ GUMBO_TAG_H3,                 IHtmlNode::TagIdH3 },
	{ GUMBO_TAG_H4,                 IHtmlNode::TagIdH4 },
	{ GUMBO_TAG_H5,                 IHtmlNode::TagIdH5 },
	{ GUMBO_TAG_H6,                 IHtmlNode::TagIdH6 },
	{ GUMBO_TAG_HGROUP,             IHtmlNode::TagIdHGroup },
	{ GUMBO_TAG_HEADER,             IHtmlNode::TagIdHeader },
	{ GUMBO_TAG_FOOTER,             IHtmlNode::TagIdFooter },
	{ GUMBO_TAG_ADDRESS,            IHtmlNode::TagIdAddress },
	{ GUMBO_TAG_P,                  IHtmlNode::TagIdP },
	{ GUMBO_TAG_HR,                 IHtmlNode::TagIdHr },
	{ GUMBO_TAG_PRE,                IHtmlNode::TagIdPre },
	{ GUMBO_TAG_BLOCKQUOTE,         IHtmlNode::TagIdBlockQuote },
	{ GUMBO_TAG_OL,                 IHtmlNode::TagIdOl },
	{ GUMBO_TAG_UL,                 IHtmlNode::TagIdUl },
	{ GUMBO_TAG_LI,                 IHtmlNode::TagIdLi },
	{ GUMBO_TAG_DL,                 IHtmlNode::TagIdDl },
	{ GUMBO_TAG_DT,                 IHtmlNode::TagIdDt },
	{ GUMBO_TAG_DD,                 IHtmlNode::TagIdDd },
	{ GUMBO_TAG_FIGURE,             IHtmlNode::TagIdFigure },
	{ GUMBO_TAG_FIGCAPTION,         IHtmlNode::TagIdFigCaption },
	{ GUMBO_TAG_MAIN,               IHtmlNode::TagIdMain },
	{ GUMBO_TAG_DIV,                IHtmlNode::TagIdDiv },
	{ GUMBO_TAG_A,                  IHtmlNode::TagIdA },
	{ GUMBO_TAG_EM,                 IHtmlNode::TagIdEm },
	{ GUMBO_TAG_STRONG,             IHtmlNode::TagIdStrong },
	{ GUMBO_TAG_SMALL,              IHtmlNode::TagIdSmall },
	{ GUMBO_TAG_S,                  IHtmlNode::TagIdS },
	{ GUMBO_TAG_CITE,               IHtmlNode::TagIdCite },
	{ GUMBO_TAG_Q,                  IHtmlNode::TagIdQ },
	{ GUMBO_TAG_DFN,                IHtmlNode::TagIdDfn },
	{ GUMBO_TAG_ABBR,               IHtmlNode::TagIdAbbr },
	{ GUMBO_TAG_DATA,               IHtmlNode::TagIdData },
	{ GUMBO_TAG_TIME,               IHtmlNode::TagIdTime },
	{ GUMBO_TAG_CODE,               IHtmlNode::TagIdCode },
	{ GUMBO_TAG_VAR,                IHtmlNode::TagIdVar },
	{ GUMBO_TAG_SAMP,               IHtmlNode::TagIdSamp },
	{ GUMBO_TAG_KBD,                IHtmlNode::TagIdKbd },
	{ GUMBO_TAG_SUB,                IHtmlNode::TagIdSub },
	{ GUMBO_TAG_SUP,                IHtmlNode::TagIdSup },
	{ GUMBO_TAG_I,                  IHtmlNode::TagIdI },
	{ GUMBO_TAG_B,                  IHtmlNode::TagIdB },
	{ GUMBO_TAG_U,                  IHtmlNode::TagIdU },
	{ GUMBO_TAG_MARK,               IHtmlNode::TagIdMark },
	{ GUMBO_TAG_RUBY,               IHtmlNode::TagIdRuby },
	{ GUMBO_TAG_RT,                 IHtmlNode::TagIdRt },
	{ GUMBO_TAG_RP,                 IHtmlNode::TagIdRp },
	{ GUMBO_TAG_BDI,                IHtmlNode::TagIdBdi },
	{ GUMBO_TAG_BDO,                IHtmlNode::TagIdBdo },
	{ GUMBO_TAG_SPAN,               IHtmlNode::TagIdSpan },
	{ GUMBO_TAG_BR,                 IHtmlNode::TagIdBr },
	{ GUMBO_TAG_WBR,                IHtmlNode::TagIdWbr },
	{ GUMBO_TAG_INS,                IHtmlNode::TagIdIns },
	{ GUMBO_TAG_DEL,                IHtmlNode::TagIdDel },
	{ GUMBO_TAG_IMAGE,              IHtmlNode::TagIdImage },
	{ GUMBO_TAG_IMG,                IHtmlNode::TagIdImg },
	{ GUMBO_TAG_IFRAME,             IHtmlNode::TagIdIframe },
	{ GUMBO_TAG_EMBED,              IHtmlNode::TagIdEmbed },
	{ GUMBO_TAG_OBJECT,             IHtmlNode::TagIdObject },
	{ GUMBO_TAG_PARAM,              IHtmlNode::TagIdParam },
	{ GUMBO_TAG_VIDEO,              IHtmlNode::TagIdVideo },
	{ GUMBO_TAG_AUDIO,              IHtmlNode::TagIdAudio },
	{ GUMBO_TAG_SOURCE,             IHtmlNode::TagIdSource },
	{ GUMBO_TAG_TRACK,              IHtmlNode::TagIdTrack },
	{ GUMBO_TAG_CANVAS,             IHtmlNode::TagIdCanvas },
	{ GUMBO_TAG_MAP,                IHtmlNode::TagIdMap },
	{ GUMBO_TAG_AREA,               IHtmlNode::TagIdArea },
	{ GUMBO_TAG_MATH,               IHtmlNode::TagIdMath },
	{ GUMBO_TAG_MI,                 IHtmlNode::TagIdMi },
	{ GUMBO_TAG_MO,                 IHtmlNode::TagIdMo },
	{ GUMBO_TAG_MN,                 IHtmlNode::TagIdMn },
	{ GUMBO_TAG_MS,                 IHtmlNode::TagIdMs },
	{ GUMBO_TAG_MTEXT,              IHtmlNode::TagIdMtext },
	{ GUMBO_TAG_MGLYPH,             IHtmlNode::TagIdMglyph },
	{ GUMBO_TAG_MALIGNMARK,         IHtmlNode::TagIdMalignMark },
	{ GUMBO_TAG_ANNOTATION_XML,     IHtmlNode::TagIdAnnotationXml },
	{ GUMBO_TAG_SVG,                IHtmlNode::TagIdSvg },
	{ GUMBO_TAG_FOREIGNOBJECT,      IHtmlNode::TagIdForeignObject },
	{ GUMBO_TAG_DESC,               IHtmlNode::TagIdDesc },
	{ GUMBO_TAG_TABLE,              IHtmlNode::TagIdTable },
	{ GUMBO_TAG_CAPTION,            IHtmlNode::TagIdCaption },
	{ GUMBO_TAG_COLGROUP,           IHtmlNode::TagIdColGroup },
	{ GUMBO_TAG_COL,                IHtmlNode::TagIdCol },
	{ GUMBO_TAG_TBODY,              IHtmlNode::TagIdTbody },
	{ GUMBO_TAG_THEAD,              IHtmlNode::TagIdThead },
	{ GUMBO_TAG_TFOOT,              IHtmlNode::TagIdTfoot },
	{ GUMBO_TAG_TR,                 IHtmlNode::TagIdTr },
	{ GUMBO_TAG_TD,                 IHtmlNode::TagIdTd },
	{ GUMBO_TAG_TH,                 IHtmlNode::TagIdTh },
	{ GUMBO_TAG_FORM,               IHtmlNode::TagIdForm },
	{ GUMBO_TAG_FIELDSET,           IHtmlNode::TagIdFieldSet },
	{ GUMBO_TAG_LEGEND,             IHtmlNode::TagIdLegend },
	{ GUMBO_TAG_LABEL,              IHtmlNode::TagIdLabel },
	{ GUMBO_TAG_INPUT,              IHtmlNode::TagIdInput },
	{ GUMBO_TAG_BUTTON,             IHtmlNode::TagIdButton },
	{ GUMBO_TAG_SELECT,             IHtmlNode::TagIdSelect },
	{ GUMBO_TAG_DATALIST,           IHtmlNode::TagIdDataList },
	{ GUMBO_TAG_OPTGROUP,           IHtmlNode::TagIdOptGroup },
	{ GUMBO_TAG_OPTION,             IHtmlNode::TagIdOption },
	{ GUMBO_TAG_TEXTAREA,           IHtmlNode::TagIdTextArea },
	{ GUMBO_TAG_KEYGEN,             IHtmlNode::TagIdKeygen },
	{ GUMBO_TAG_OUTPUT,             IHtmlNode::TagIdOutput },
	{ GUMBO_TAG_PROGRESS,           IHtmlNode::TagIdProgress },
	{ GUMBO_TAG_METER,              IHtmlNode::TagIdMeter },
	{ GUMBO_TAG_DETAILS,            IHtmlNode::TagIdDetails },
	{ GUMBO_TAG_SUMMARY,            IHtmlNode::TagIdSummary },
	{ GUMBO_TAG_MENU,               IHtmlNode::TagIdMenu },
	{ GUMBO_TAG_MENUITEM,           IHtmlNode::TagIdMenuItem },
	{ GUMBO_TAG_APPLET,             IHtmlNode::TagIdApplet },
	{ GUMBO_TAG_ACRONYM,            IHtmlNode::TagIdAcronym },
	{ GUMBO_TAG_BGSOUND,            IHtmlNode::TagIdBgSound },
	{ GUMBO_TAG_DIR,                IHtmlNode::TagIdDir },
	{ GUMBO_TAG_FRAME,              IHtmlNode::TagIdFrame },
	{ GUMBO_TAG_FRAMESET,           IHtmlNode::TagIdFrameSet },
	{ GUMBO_TAG_NOFRAMES,           IHtmlNode::TagIdNoFrames },
	{ GUMBO_TAG_ISINDEX,            IHtmlNode::TagIdIsIndex },
	{ GUMBO_TAG_LISTING,            IHtmlNode::TagIdListing },
	{ GUMBO_TAG_XMP,                IHtmlNode::TagIdXmp },
	{ GUMBO_TAG_NEXTID,             IHtmlNode::TagIdNextId },
	{ GUMBO_TAG_NOEMBED,            IHtmlNode::TagIdNoEmbed },
	{ GUMBO_TAG_PLAINTEXT,          IHtmlNode::TagIdPlainText },
	{ GUMBO_TAG_RB,                 IHtmlNode::TagIdRb },
	{ GUMBO_TAG_STRIKE,             IHtmlNode::TagIdStrike },
	{ GUMBO_TAG_BASEFONT,           IHtmlNode::TagIdBaseFont },
	{ GUMBO_TAG_BIG,                IHtmlNode::TagIdBig },
	{ GUMBO_TAG_BLINK,              IHtmlNode::TagIdBlink },
	{ GUMBO_TAG_CENTER,             IHtmlNode::TagIdCenter },
	{ GUMBO_TAG_FONT,               IHtmlNode::TagIdFont },
	{ GUMBO_TAG_MARQUEE,            IHtmlNode::TagIdMarquee },
	{ GUMBO_TAG_MULTICOL,           IHtmlNode::TagIdMulticol },
	{ GUMBO_TAG_NOBR,               IHtmlNode::TagIdNoBr },
	{ GUMBO_TAG_SPACER,             IHtmlNode::TagIdSpacer },
	{ GUMBO_TAG_TT,                 IHtmlNode::TagIdTt },
	{ GUMBO_TAG_RTC,                IHtmlNode::TagIdRtc },
	{ GUMBO_TAG_UNKNOWN,            IHtmlNode::TagIdUnknown }
};

const QMap<int, IHtmlNode::NodeType> s_nodeTypeMapping
{
	{ GUMBO_NODE_DOCUMENT, IHtmlNode::NodeTypeDocument },
	{ GUMBO_NODE_ELEMENT, IHtmlNode::NodeTypeElement },
	{ GUMBO_NODE_TEXT, IHtmlNode::NodeTypeText },
	{ GUMBO_NODE_CDATA, IHtmlNode::NodeTypeCData },
	{ GUMBO_NODE_COMMENT, IHtmlNode::NodeTypeComment },
	{ GUMBO_NODE_WHITESPACE, IHtmlNode::NodeTypeWhiteSpace },
	{ GUMBO_NODE_TEMPLATE, IHtmlNode::NodeTypeTemplate }
};

}

namespace CrawlerEngine
{

GumboHtmlNode::GumboHtmlNode(GumboNode* node)
	: m_node(node)
{
}

IHtmlNode::TagId GumboHtmlNode::tagId() const
{
	ASSERT(m_node->v.element.tag == GUMBO_NODE_ELEMENT);

	return s_tagIdMapping[m_node->v.element.tag];
}

IHtmlNode::NodeType GumboHtmlNode::type() const
{
	return s_nodeTypeMapping[m_node->type];
}

QByteArray GumboHtmlNode::text() const
{
	if (m_node->type == GUMBO_NODE_TEXT)
	{
		return m_node->v.text.text;
	}

	if (m_node->v.element.children.length > 0)
	{
		const GumboNode* textNode = static_cast<GumboNode*>(m_node->v.element.children.data[0]);

		if (textNode->type == GUMBO_NODE_TEXT)
		{
			return textNode->v.text.text;
		}
	}

	return QByteArray();
}

QString GumboHtmlNode::attribute(const QByteArray& attributeName) const
{
	ASSERT(type() == IHtmlNode::NodeTypeElement);

	const GumboAttribute* attr = gumbo_get_attribute(&m_node->v.element.attributes, attributeName.data());

	return QString(attr->value);
}

bool GumboHtmlNode::hasAttribute(const QByteArray& attributeName) const
{
	ASSERT(type() == IHtmlNode::NodeTypeElement);
	return gumbo_get_attribute(&m_node->v.element.attributes, attributeName.data()) != nullptr;
}

GumboHtmlNode::operator bool() const
{
	return m_node != nullptr;
}

HtmlNodeSharedPtr GumboHtmlNode::findSubNode(TagId tagId, unsigned startIndexWhithinParent) const
{
	DEBUG_ASSERT(m_node->type == GUMBO_NODE_ELEMENT || m_node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &m_node->v.element.children;
	GumboNode* searchingNode = nullptr;

	DEBUG_ASSERT(startIndexWhithinParent <= children->length);

	for (unsigned i = startIndexWhithinParent; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && s_tagIdMapping[child->v.element.tag] == tagId)
		{
			searchingNode = child;
			break;
		}
	}

	return std::make_shared<GumboHtmlNode>(searchingNode);
}

std::vector<HtmlNodeSharedPtr> GumboHtmlNode::subNodes(TagId tagId) const
{
	DEBUG_ASSERT(m_node->type == GUMBO_NODE_ELEMENT || m_node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &m_node->v.element.children;
	std::vector<HtmlNodeSharedPtr> nodes;

	for (unsigned i = 0; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && s_tagIdMapping[child->v.element.tag] == tagId)
		{
			nodes.push_back(std::make_shared<GumboHtmlNode>(child));
		}
	}

	return nodes;
}

}