#include "html_node_model.h"
#include "ihtml_parser.h"

namespace 
{

using namespace CrawlerEngine;

QVector<void*> path(const IHtmlNodeCountedPtr& node, const IHtmlNodeCountedPtr& root)
{
	QVector<void*> result;

	IHtmlNodeCountedPtr current = node;
	while (current)
	{
		result.push_back(current->data());
		if (current->data() == root->data())
		{
			break;
		}
		current = current->parent();
	}

	std::reverse(result.begin(), result.end());
	return result;
}

QMap<IHtmlNode::TagId, QString> s_tagToName
{
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
	IHtmlNodeCountedPtr node1 = toHtmlNode(ni1);
	IHtmlNodeCountedPtr node2 = toHtmlNode(ni2);

	if (node1->data() == node2->data())
		return QXmlNodeModelIndex::Is;

	auto root = m_parser->root();
	auto path1 = path(node1, root);
	auto path2 = path(node2, root);

	QSet<void*> set1;
	QSet<void*> set2;

	for (auto i : path1)
	{
		set1.insert(i);
	}

	for (auto i : path2)
	{
		set2.insert(i);
	}

	QSet<void*> intersection = set1.intersect(set2);
	ASSERT(!intersection.isEmpty());

	void* commonParent = nullptr;

	// move from back to begin to find a node that contains in both paths
	for (int i = path1.size() - 1; i >= 0; --i)
	{
		if (intersection.contains(path1[i]))
		{
			commonParent = path1[i];
			break;
		}
	}

	ASSERT(commonParent != nullptr);

	int index = path1.indexOf(commonParent);
	ASSERT(path2.indexOf(commonParent) == index);

	if (index == path1.size() - 1 && index != path2.size() - 1)
	{
		return QXmlNodeModelIndex::Precedes;
	}

	if (index == path2.size() - 1 && index != path1.size() - 1)
	{
		return QXmlNodeModelIndex::Follows;
	}

	auto ch1 = m_parser->fromData(path1[index + 1]);
	auto ch2 = m_parser->fromData(path2[index + 1]);

	return ch1->childIndex() < ch2->childIndex() ? QXmlNodeModelIndex::Precedes : QXmlNodeModelIndex::Follows;
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

	IHtmlNodeCountedPtr node = toHtmlNode(ni);
	switch (node->type()) 
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

	IHtmlNodeCountedPtr node = toHtmlNode(ni);
	const QString name = tagToName(node->tagId());
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
	IHtmlNodeCountedPtr node = toHtmlNode(n);
	while (node->parent())
	{
		node = node->parent();
	}

	return fromHtmlNode(node);
}

QString HtmlNodeModel::stringValue(const QXmlNodeModelIndex& n) const
{
	if (n.additionalData() == s_attributeId)
	{
		IHtmlAttributeCountedPtr attribute = toHtmlAttribute(n);
		const QString attrValue = attribute->value().toLower();
		return attrValue;
	}

	IHtmlNodeCountedPtr node = toHtmlNode(n);
	if (node->type() == IHtmlNode::NodeTypeText)
	{
		return node->text();
	}

	// see documentation of this method, it should return different values depending on node type
	// also text() method returns text of the first child text node what is incorrect

	return node->text();
}

QVariant HtmlNodeModel::typedValue(const QXmlNodeModelIndex& node) const
{
	// ASSERT(node.additionalData() != s_attributeId || !"Not implemented for attributes yet");

	// probably incorrect implementation
	return stringValue(node);
}

QVector<QXmlNodeModelIndex> HtmlNodeModel::attributes(const QXmlNodeModelIndex& element) const
{
	IHtmlNodeCountedPtr node = toHtmlNode(element);
	QVector<QXmlNodeModelIndex> result;

	for (int i = 0; i < node->attributesCount(); ++i)
	{
		IHtmlAttributeCountedPtr attr = node->attribute(i);
		result.push_back(fromHtmlAttribute(attr));
	}

	return result;
	
}

QXmlNodeModelIndex HtmlNodeModel::nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis axis, const QXmlNodeModelIndex& origin) const
{
	ASSERT(origin.additionalData() != s_attributeId || !"Not implemented for attributes yet");
	IHtmlNodeCountedPtr node = toHtmlNode(origin);

	switch (axis)
	{
	case Parent:
	{
		const IHtmlNodeCountedPtr parent = node->parent();
		return parent ? fromHtmlNode(parent) : QXmlNodeModelIndex();
	}
		
	case FirstChild:
	{
		const IHtmlNodeCountedPtr firstChild = node->firstChild();
		return firstChild ? fromHtmlNode(firstChild) : QXmlNodeModelIndex();
	}
		
	case PreviousSibling:
	{
		const IHtmlNodeCountedPtr prevSibling = node->prevSibling();
		return prevSibling ? fromHtmlNode(prevSibling) : QXmlNodeModelIndex();
	}
	case NextSibling:
	{
		const IHtmlNodeCountedPtr nextSibling = node->nextSibling();
		return nextSibling ? fromHtmlNode(nextSibling) : QXmlNodeModelIndex();
	}
	}

	return QXmlNodeModelIndex();
}

IHtmlNodeCountedPtr HtmlNodeModel::toHtmlNode(const QXmlNodeModelIndex& index) const
{
	return m_parser->fromData((void*)index.data());
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