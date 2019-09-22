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

QXmlNamePool s_pool;
/*
TagIdHtml,
TagIdHead,
TagIdTitle,
TagIdBase,
TagIdLink,
TagIdMeta,
TagIdStyle,
TagIdScript,
TagIdNoScript,
TagIdTemplate,
TagIdBody,
TagIdArticle,
TagIdSection,
TagIdNav,
TagIdAside,
TagIdH1,
TagIdH2,
TagIdH3,
TagIdH4,
TagIdH5,
TagIdH6,
TagIdHGroup,
TagIdHeader,
TagIdFooter,
TagIdAddress,
TagIdP,
TagIdHr,
TagIdPre,
TagIdBlockQuote,
TagIdOl,
TagIdUl,
TagIdLi,
TagIdDl,
TagIdDt,
TagIdDd,
TagIdFigure,
TagIdFigCaption,
TagIdMain,
TagIdDiv,
TagIdA,
TagIdEm,
TagIdStrong,
TagIdSmall,
TagIdS,
TagIdCite,
TagIdQ,
TagIdDfn,
TagIdAbbr,
TagIdData,
TagIdTime,
TagIdCode,
TagIdVar,
TagIdSamp,
TagIdKbd,
TagIdSub,
TagIdSup,
TagIdI,
TagIdB,
TagIdU,
TagIdMark,
TagIdRuby,
TagIdRt,
TagIdRp,
TagIdBdi,
TagIdBdo,
TagIdSpan,
TagIdBr,
TagIdWbr,
TagIdIns,
TagIdDel,
TagIdImage,
TagIdImg,
TagIdIframe,
TagIdEmbed,
TagIdObject,
TagIdParam,
TagIdVideo,
TagIdAudio,
TagIdSource,
TagIdTrack,
TagIdCanvas,
TagIdMap,
TagIdArea,
TagIdMath,
TagIdMi,
TagIdMo,
TagIdMn,
TagIdMs,
TagIdMtext,
TagIdMglyph,
TagIdMalignMark,
TagIdAnnotationXml,
TagIdSvg,
TagIdForeignObject,
TagIdDesc,
TagIdTable,
TagIdCaption,
TagIdColGroup,
TagIdCol,
TagIdTbody,
TagIdThead,
TagIdTfoot,
TagIdTr,
TagIdTd,
TagIdTh,
TagIdForm,
TagIdFieldSet,
TagIdLegend,
TagIdLabel,
TagIdInput,
TagIdButton,
TagIdSelect,
TagIdDataList,
TagIdOptGroup,
TagIdOption,
TagIdTextArea,
TagIdKeygen,
TagIdOutput,
TagIdProgress,
TagIdMeter,
TagIdDetails,
TagIdSummary,
TagIdMenu,
TagIdMenuItem,
TagIdApplet,
TagIdAcronym,
TagIdBgSound,
TagIdDir,
TagIdFrame,
TagIdFrameSet,
TagIdNoFrames,
TagIdIsIndex,
TagIdListing,
TagIdXmp,
TagIdNextId,
TagIdNoEmbed,
TagIdPlainText,
TagIdRb,
TagIdStrike,
TagIdBaseFont,
TagIdBig,
TagIdBlink,
TagIdCenter,
TagIdFont,
TagIdMarquee,
TagIdMulticol,
TagIdNoBr,
TagIdSpacer,
TagIdTt,
TagIdRtc,
TagIdUnknown
*/
QMap<IHtmlNode::TagId, QString> s_tagToName
{
	{ IHtmlNode::TagIdA, "a" }
	// TODO: add the rest
};

QMap<QString, IHtmlNode::TagId> s_nameToTag
{
	{ "a", IHtmlNode::TagIdA }
	// TODO: add the rest
};

}

namespace CrawlerEngine
{

HtmlNodeModel::HtmlNodeModel(const IHtmlParser* parser)
	: QAbstractXmlNodeModel()
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
	const QString tagName = id.localName(s_pool);
	IHtmlNode::TagId tagId = s_nameToTag.value(tagName, IHtmlNode::TagIdUnknown);
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
	IHtmlNodeCountedPtr node = toHtmlNode(ni);
	const QString name = s_tagToName.value(node->tagId(), QString());
	return name.isEmpty() ? QXmlName() : QXmlName(s_pool, name);
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
	// probably incorrect implementation
	return stringValue(node);
}

QVector<QXmlNodeModelIndex> HtmlNodeModel::attributes(const QXmlNodeModelIndex& element) const
{
	Q_UNUSED(element);
	// attributes also should be represented by QXmlNodeModelIndex, how to achieve this?
	return QVector<QXmlNodeModelIndex>();
}

QXmlNodeModelIndex HtmlNodeModel::nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis axis, const QXmlNodeModelIndex& origin) const
{
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

QXmlNodeModelIndex HtmlNodeModel::fromHtmlNode(const IHtmlNodeCountedPtr& node) const
{
	if (!node) {
		return QXmlNodeModelIndex();
	}

	ASSERT((void*)node->data() != nullptr);
	
	return createIndex(node->data());
}

QXmlNodeModelIndex HtmlNodeModel::rootIndex() const
{
	return fromHtmlNode(m_parser->root());
}

}