#pragma once

namespace CrawlerEngine
{

class IHtmlNode;
class IHtmlAttribute;

using IHtmlNodeCountedPtr = Common::counted_ptr<IHtmlNode>;
using IHtmlAttributeCountedPtr = Common::counted_ptr<IHtmlAttribute>;

class IHtmlAttribute
{
public:
	virtual ~IHtmlAttribute() = default;
	virtual IHtmlAttributeCountedPtr nextSibling() const = 0;
	virtual IHtmlAttributeCountedPtr prevSibling() const = 0;
	virtual QString name() const = 0;
	virtual QString value() const = 0;
	virtual void* data() const = 0;
};

class IHtmlNode
{
public:
	enum TagId
	{
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
	};

	enum NodeType
	{
		NodeTypeDocument,
		NodeTypeElement,
		NodeTypeText,
		NodeTypeCData,
		NodeTypeComment,
		NodeTypeWhiteSpace,
		NodeTypeTemplate,
		NodeTypeInvalid
	};

	virtual ~IHtmlNode() = default;

	// if this node is actually tag
	virtual TagId tagId() const = 0;
	virtual NodeType type() const = 0;
	virtual QByteArray text() const = 0;

	// if this node has "type() == NodeTypeElement"
	virtual QString attribute(const QByteArray& attributeName) const = 0;
	virtual bool hasAttribute(const QByteArray& attributeName) const = 0;

	virtual operator bool() const = 0;

	virtual IHtmlNodeCountedPtr parent() const = 0;
	virtual IHtmlNodeCountedPtr firstChild() const = 0;
	virtual IHtmlNodeCountedPtr nextSibling() const = 0;
	virtual IHtmlNodeCountedPtr prevSibling() const = 0;
	virtual int childIndex() const = 0;

	virtual int attributesCount() const = 0;
	virtual IHtmlAttributeCountedPtr attribute(int index) const = 0;

	virtual IHtmlNodeCountedPtr firstMatchSubNode(TagId tagId, unsigned startIndexWhithinParent = 0) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodes(TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(TagId tagId) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> matchSubNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const = 0;
	virtual std::vector<IHtmlNodeCountedPtr> children() const = 0;
	virtual QByteArray cutSubNodesAndGetPlainText() const = 0;
	virtual IHtmlNodeCountedPtr childNodeByAttributeValue(TagId tagId, std::pair<const char*, const char*> expectedAttributes) const = 0;
	virtual IHtmlNodeCountedPtr childNodeByAttributesValues(TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const = 0;

	virtual void* data() const = 0;
};

}