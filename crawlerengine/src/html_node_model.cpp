#include "html_node_model.h"
#include "ihtml_parser.h"

namespace 
{

using namespace CrawlerEngine;

QMap<IHtmlNode::TagId, QString> s_tagToName
{
	{ IHtmlNode::TagIdUndef, "undef" },
	{ IHtmlNode::TagIdText, "text" },
	{ IHtmlNode::TagIdComment, "comment" },
	{ IHtmlNode::TagIdDocType, "doctype" },

	{ IHtmlNode::TagIdA, "a" },
	{ IHtmlNode::TagIdHtml, "html" },
	{ IHtmlNode::TagIdHead, "head" },
	{ IHtmlNode::TagIdTitle, "title" },
	{ IHtmlNode::TagIdBase, "base" },
	{ IHtmlNode::TagIdBody, "body" },
	{ IHtmlNode::TagIdLink, "link" },
	{ IHtmlNode::TagIdMeta, "meta" },
	{ IHtmlNode::TagIdStyle, "style" },
	{ IHtmlNode::TagIdScript, "script" },
	{ IHtmlNode::TagIdNoScript, "noscript" },
	{ IHtmlNode::TagIdTemplate, "template" },
	{ IHtmlNode::TagIdArticle, "article" },
	{ IHtmlNode::TagIdSection, "section" },
	{ IHtmlNode::TagIdNav, "nav" },
	{ IHtmlNode::TagIdAside, "aside" },
	{ IHtmlNode::TagIdH1, "h1" },
	{ IHtmlNode::TagIdH2, "h2" },
	{ IHtmlNode::TagIdH3, "h3" },
	{ IHtmlNode::TagIdH4, "h4" },
	{ IHtmlNode::TagIdH5, "h5" },
	{ IHtmlNode::TagIdH6, "h6" },
	{ IHtmlNode::TagIdHGroup, "group" },
	{ IHtmlNode::TagIdHeader, "header" },
	{ IHtmlNode::TagIdFooter, "footer" },
	{ IHtmlNode::TagIdAddress, "address" },
	{ IHtmlNode::TagIdP, "ip" },
	{ IHtmlNode::TagIdHr, "hr" },
	{ IHtmlNode::TagIdPre, "pre" },
	{ IHtmlNode::TagIdBlockQuote, "blockquote" },
	{ IHtmlNode::TagIdOl, "ol" },
	{ IHtmlNode::TagIdUl, "ul" },
	{ IHtmlNode::TagIdLi, "li" },
	{ IHtmlNode::TagIdDl, "dl" },
	{ IHtmlNode::TagIdDt, "dt" },
	{ IHtmlNode::TagIdDd, "dd" },
	{ IHtmlNode::TagIdFigure, "figure" },
	{ IHtmlNode::TagIdFigCaption, "caption" },
	{ IHtmlNode::TagIdMain, "main" },
	{ IHtmlNode::TagIdDiv, "div" },
	{ IHtmlNode::TagIdA, "a" },
	{ IHtmlNode::TagIdEm, "em" },
	{ IHtmlNode::TagIdStrong, "strong" },
	{ IHtmlNode::TagIdSmall, "small" },
	{ IHtmlNode::TagIdS, "s" },
	{ IHtmlNode::TagIdCite, "cite" },
	{ IHtmlNode::TagIdQ, "q" },
	{ IHtmlNode::TagIdDfn, "fn" },
	{ IHtmlNode::TagIdAbbr, "abbr" },
	{ IHtmlNode::TagIdData, "data" },
	{ IHtmlNode::TagIdTime, "time" },
	{ IHtmlNode::TagIdCode, "code" },
	{ IHtmlNode::TagIdVar, "var" },
	{ IHtmlNode::TagIdSamp, "samp" },
	{ IHtmlNode::TagIdKbd, "kbd" },
	{ IHtmlNode::TagIdSub, "sub"},
	{ IHtmlNode::TagIdSup, "sup"},
	{ IHtmlNode::TagIdI, "i"},
	{ IHtmlNode::TagIdB, "b"},
	{ IHtmlNode::TagIdU, "u"},
	{ IHtmlNode::TagIdMark, "mark"},
	{ IHtmlNode::TagIdRuby, "ruby"},
	{ IHtmlNode::TagIdRt, "rt"},
	{ IHtmlNode::TagIdRp, "rp"},
	{ IHtmlNode::TagIdBdi, "bdi"},
	{ IHtmlNode::TagIdBdo, "bdo"},
	{ IHtmlNode::TagIdSpan, "span"},
	{ IHtmlNode::TagIdBr, "br"},
	{ IHtmlNode::TagIdWbr, "wbr"},
	{ IHtmlNode::TagIdIns, "ins"},
	{ IHtmlNode::TagIdDel, "del"},
	{ IHtmlNode::TagIdImage, "image"},
	{ IHtmlNode::TagIdImg, "img"},
	{ IHtmlNode::TagIdIframe, "iframe"},
	{ IHtmlNode::TagIdEmbed, "embed"},
	{ IHtmlNode::TagIdObject, "object"},
	{ IHtmlNode::TagIdParam, "param"},
	{ IHtmlNode::TagIdVideo, "video"},
	{ IHtmlNode::TagIdAudio, "audio"},
	{ IHtmlNode::TagIdSource, "source"},
	{ IHtmlNode::TagIdTrack, "track"},
	{ IHtmlNode::TagIdCanvas, "canvas"},
	{ IHtmlNode::TagIdMap, "map"},
	{ IHtmlNode::TagIdArea, "area"},
	{ IHtmlNode::TagIdMath, "math"},
	{ IHtmlNode::TagIdMi, "mi"},
	{ IHtmlNode::TagIdMo, "mo"},
	{ IHtmlNode::TagIdMn, "mn"},
	{ IHtmlNode::TagIdMs, "ms"},
	{ IHtmlNode::TagIdMtext, "mtext"},
	{ IHtmlNode::TagIdMglyph, "mglyph"},
	{ IHtmlNode::TagIdMalignMark, "malgnmark"},
	{ IHtmlNode::TagIdAnnotationXml, "annotationxml"},
	{ IHtmlNode::TagIdSvg, "svg"},
	{ IHtmlNode::TagIdForeignObject, "foreignobject"},
	{ IHtmlNode::TagIdDesc, "desc"},
	{ IHtmlNode::TagIdTable, "table"},
	{ IHtmlNode::TagIdCaption, "caption"},
	{ IHtmlNode::TagIdColGroup, "colgroup"},
	{ IHtmlNode::TagIdCol, "col"},
	{ IHtmlNode::TagIdTbody, "tbody"},
	{ IHtmlNode::TagIdThead, "thead"},
	{ IHtmlNode::TagIdTfoot, "tfoot"},
	{ IHtmlNode::TagIdTr, "tr"},
	{ IHtmlNode::TagIdTd, "td"},
	{ IHtmlNode::TagIdTh, "th"},
	{ IHtmlNode::TagIdForm, "form"},
	{ IHtmlNode::TagIdFieldSet, "fieldset"},
	{ IHtmlNode::TagIdLegend, "legend"},
	{ IHtmlNode::TagIdLabel, "label"},
	{ IHtmlNode::TagIdInput, "input"},
	{ IHtmlNode::TagIdButton, "button"},
	{ IHtmlNode::TagIdSelect, "select"},
	{ IHtmlNode::TagIdDataList, "datalist"},
	{ IHtmlNode::TagIdOptGroup, "optgroup"},
	{ IHtmlNode::TagIdOption, "option"},
	{ IHtmlNode::TagIdTextArea, "textarea"},
	{ IHtmlNode::TagIdKeygen, "keygen"},
	{ IHtmlNode::TagIdOutput, "output"},
	{ IHtmlNode::TagIdProgress, "progress"},
	{ IHtmlNode::TagIdMeter, "meter"},
	{ IHtmlNode::TagIdDetails, "details"},
	{ IHtmlNode::TagIdSummary, "summary"},
	{ IHtmlNode::TagIdMenu, "menu"},
	{ IHtmlNode::TagIdMenuItem, "menuitem"},
	{ IHtmlNode::TagIdApplet, "applet"},
	{ IHtmlNode::TagIdAcronym, "acronym"},
	{ IHtmlNode::TagIdBgSound, "sound"},
	{ IHtmlNode::TagIdDir, "dir"},
	{ IHtmlNode::TagIdFrame, "frame"},
	{ IHtmlNode::TagIdFrameSet, "frameset"},
	{ IHtmlNode::TagIdNoFrames, "noframes"},
	{ IHtmlNode::TagIdIsIndex, "isindex"},
	{ IHtmlNode::TagIdListing, "listing"},
	{ IHtmlNode::TagIdXmp, "xmp"},
	{ IHtmlNode::TagIdNextId, "nextid"},
	{ IHtmlNode::TagIdNoEmbed, "noembed"},
	{ IHtmlNode::TagIdPlainText, "plaintext"},
	{ IHtmlNode::TagIdRb, "rgb"},
	{ IHtmlNode::TagIdStrike, "strike"},
	{ IHtmlNode::TagIdBaseFont, "basefont"},
	{ IHtmlNode::TagIdBig, "big"},
	{ IHtmlNode::TagIdBlink, "blink"},
	{ IHtmlNode::TagIdCenter, "center"},
	{ IHtmlNode::TagIdFont, "font"},
	{ IHtmlNode::TagIdMarquee, "marquee"},
	{ IHtmlNode::TagIdMulticol, "multicol"},
	{ IHtmlNode::TagIdNoBr, "nobr"},
	{ IHtmlNode::TagIdSpacer, "spacer"},
	{ IHtmlNode::TagIdTt, "tt"},
	{ IHtmlNode::TagIdRtc, "rtc"},
	{ IHtmlNode::TagIdUnknown, "unknown"},
};


QMap<QString, IHtmlNode::TagId> s_nameToTag;

const QString tagToName(IHtmlNode::TagId tagId)
{
	const QString result = s_tagToName.value(tagId);
	DEBUG_ASSERT(!result.isEmpty());
	return result;
}

IHtmlNode::TagId nameToTag(const QString name)
{
	if (s_nameToTag.isEmpty())
	{
		for (auto it = s_tagToName.keyBegin(); it != s_tagToName.keyEnd(); ++it)
		{
			s_nameToTag.insert(s_tagToName.value(*it), *it);
		}
	}

	DEBUG_ASSERT(s_nameToTag.contains(name));

	return s_nameToTag.value(name);
}

int s_nodeId = 0;
int s_attributeId = 1;

}

namespace CrawlerEngine
{

HtmlNodeModel::HtmlNodeModel(QXmlNamePool pool, const IHtmlParser* parser)
	: QAbstractXmlNodeModel()
	, m_pool(pool)
	, m_parser(parser)
	, m_cachedNode1(m_parser->emptyNode())
	, m_cachedNode2(m_parser->emptyNode())
{
}

QUrl HtmlNodeModel::baseUri(const QXmlNodeModelIndex& index) const
{
	Q_UNUSED(index);
	// TODO: implement
	return QUrl();
}

QXmlNodeModelIndex::DocumentOrder HtmlNodeModel::compareOrder(const QXmlNodeModelIndex& ni1, const QXmlNodeModelIndex& ni2) const
{
	toHtmlNode(ni1, m_cachedNode1);
	toHtmlNode(ni2, m_cachedNode2);

	auto compareResult = m_cachedNode1->compare(m_cachedNode2);
	if (compareResult == 0)
	{
		return QXmlNodeModelIndex::Is;
	}
	// -1 if node2 is before node1, 0 if nodes are the same, and 1 if node2 is after node1
	return compareResult == 1 ? QXmlNodeModelIndex::Follows : QXmlNodeModelIndex::Precedes;
}

QUrl HtmlNodeModel::documentUri(const QXmlNodeModelIndex& n) const
{
	Q_UNUSED(n);
	// TODO: implement
	return QUrl();
}

QXmlNodeModelIndex HtmlNodeModel::elementById(const QXmlName& id) const
{
	const QString tagName = id.localName(m_pool);
	IHtmlNode::TagId tagId = nameToTag(tagName);
	if (tagId == IHtmlNode::TagIdUnknown)
	{
		return QXmlNodeModelIndex();
	}

	// PROBABLY WRONG IMPLEMENTATION, debug to figure out what contains QXmlName
	const IHtmlNodeCountedPtr result = m_parser->firstMatchNode(tagId);
	return result ? fromHtmlNode(result) : QXmlNodeModelIndex();
}

QXmlNodeModelIndex::NodeKind HtmlNodeModel::kind(const QXmlNodeModelIndex& ni) const
{
	if (ni.additionalData() == s_attributeId)
	{
		return QXmlNodeModelIndex::Attribute;
	}

	toHtmlNode(ni, m_cachedNode1);
	switch (m_cachedNode1->type())
	{
	case IHtmlNode::NodeTypeDocument:
		return QXmlNodeModelIndex::Document;
	case IHtmlNode::NodeTypeElement:
		return QXmlNodeModelIndex::Element;
	case IHtmlNode::NodeTypeText:
		return QXmlNodeModelIndex::Text;
	case IHtmlNode::NodeTypeCData:
		return QXmlNodeModelIndex::ProcessingInstruction; // ???
	case IHtmlNode::NodeTypeComment:
		return QXmlNodeModelIndex::Comment;
	case IHtmlNode::NodeTypeWhiteSpace:
		return QXmlNodeModelIndex::Text; // ??
	case IHtmlNode::NodeTypeTemplate:
	case IHtmlNode::NodeTypeInvalid:
		return QXmlNodeModelIndex::Comment; // ??
	default:
		return QXmlNodeModelIndex::Comment; // ??
	}
}

QXmlName HtmlNodeModel::name(const QXmlNodeModelIndex& ni) const
{
	if (ni.additionalData() == s_attributeId)
	{
		IHtmlAttributeCountedPtr attribute = toHtmlAttribute(ni);
		const QString attrName = attribute->name().toLower();
		return  QXmlName(m_pool, attrName);
	}

	toHtmlNode(ni, m_cachedNode1);
	const QString name = tagToName(m_cachedNode1->tagId());
	return name.isEmpty() ? QXmlName() : QXmlName(m_pool, name);
}

QVector<QXmlName> HtmlNodeModel::namespaceBindings(const QXmlNodeModelIndex& n) const
{
	Q_UNUSED(n);
	// TODO: implement
	return QVector<QXmlName>();
}

QVector<QXmlNodeModelIndex> HtmlNodeModel::nodesByIdref(const QXmlName& idref) const
{
	Q_UNUSED(idref);
	// TODO: implement
	return QVector<QXmlNodeModelIndex>();
}

QXmlNodeModelIndex HtmlNodeModel::root(const QXmlNodeModelIndex& n) const
{
	toHtmlNode(n, m_cachedNode1);
	while (m_cachedNode1->data() != nullptr)
	{
		m_cachedNode1->parent(m_cachedNode1);
	}

	return fromHtmlNode(m_cachedNode1);
}

QString HtmlNodeModel::stringValue(const QXmlNodeModelIndex& n) const
{
	if (n.additionalData() == s_attributeId)
	{
		IHtmlAttributeCountedPtr attribute = toHtmlAttribute(n);
		const QString attrValue = attribute->value().toLower();
		return attrValue;
	}

	toHtmlNode(n, m_cachedNode1);
	if (m_cachedNode1->type() == IHtmlNode::NodeTypeText)
	{
		return m_cachedNode1->text();
	}

	// see documentation of this method, it should return different values depending on node type
	// also text() method returns text of the first child text node what is incorrect

	return m_cachedNode1->text();
}

QVariant HtmlNodeModel::typedValue(const QXmlNodeModelIndex& node) const
{
	// ASSERT(node.additionalData() != s_attributeId || !"Not implemented for attributes yet");

	// probably incorrect implementation
	return stringValue(node);
}

QVector<QXmlNodeModelIndex> HtmlNodeModel::attributes(const QXmlNodeModelIndex& element) const
{
	toHtmlNode(element, m_cachedNode1);
	QVector<QXmlNodeModelIndex> result;

	for (int i = 0; i < m_cachedNode1->attributesCount(); ++i)
	{
		IHtmlAttributeCountedPtr attr = m_cachedNode1->attribute(i);
		result.push_back(fromHtmlAttribute(attr));
	}

	return result;
	
}

QXmlNodeModelIndex HtmlNodeModel::nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis axis, const QXmlNodeModelIndex& origin) const
{
	ASSERT(origin.additionalData() != s_attributeId || !"Not implemented for attributes yet");
	toHtmlNode(origin, m_cachedNode1);

	QString name = tagToName(m_cachedNode1->tagId());
	Q_UNUSED(name);

	switch (axis)
	{
	case Parent:
	{
		m_cachedNode1->parent(m_cachedNode1);
		break;
	}
		
	case FirstChild:
	{
		m_cachedNode1->firstChild(m_cachedNode1);
		break;
	}
		
	case PreviousSibling:
	{
		m_cachedNode1->prevSibling(m_cachedNode1);
		break;
	}
	case NextSibling:
	{
		m_cachedNode1->nextSibling(m_cachedNode1);
		break;
	}
	}

	if (m_cachedNode1->data() != nullptr)
	{
		QString result = tagToName(m_cachedNode1->tagId());
		Q_UNUSED(result);
		result += "";
	}

	return m_cachedNode1->data() != nullptr ? fromHtmlNode(m_cachedNode1) : QXmlNodeModelIndex();
}

void HtmlNodeModel::toHtmlNode(const QXmlNodeModelIndex& index, IHtmlNodeCountedPtr& node) const
{
	return node->setData((void*)index.data());
}

IHtmlAttributeCountedPtr HtmlNodeModel::toHtmlAttribute(const QXmlNodeModelIndex& index) const
{
	return m_parser->attributeFromData((void*)index.data());
}

QXmlNodeModelIndex HtmlNodeModel::fromHtmlNode(const IHtmlNodeCountedPtr& node) const
{
	if (!node) {
		return QXmlNodeModelIndex();
	}

	ASSERT((void*)node->data() != nullptr);
	
	return createIndex(node->data(), s_nodeId);
}

QXmlNodeModelIndex HtmlNodeModel::fromHtmlAttribute(const IHtmlAttributeCountedPtr& attribute) const
{
	if (!attribute) {
		return QXmlNodeModelIndex();
	}

	ASSERT((void*)attribute->data() != nullptr);

	return createIndex(attribute->data(), s_attributeId);
}

QXmlNodeModelIndex HtmlNodeModel::rootIndex() const
{
	return fromHtmlNode(m_parser->root());
}

}