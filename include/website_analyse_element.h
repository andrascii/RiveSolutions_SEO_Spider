#pragma once

namespace QuickieWebBot
{

struct WebsiteAnalyseElement
{
	QUrl url;

	QString content;

	QString metaRefresh;
	QString metaRobots;

	QString redirectedUrl;
	QString serverResponse;

	QString title;
	QString metaDescription;
	QString metaKeywords;

	QString firstH1;
	QString secondH1;

	QString firstH2;
	QString secondH2;

	QString canonicalLinkElement;

	int statusCode;
	int titleLength;

	int metaDescriptionLength;
	int metaKeywordsLength;

	int firstH1Length;
	int secondH1Length;

	int firstH2Length;
	int secondH2Length;

	int pageSizeBytes;
	int wordCount;

	size_t pageHash;
};

enum WebSiteAnalyseElementMemberGetter
{
	GetterUrl,
	GetterContent,
	GetterMetaRefresh,
	GetterMetaRobots,
	GetterRedirectedUrl,
	GetterServerResponse,
	GetterTitle,
	GetterMetaDescription,
	GetterMetaKeywords,
	GetterFirstH1,
	GetterSecondH1,
	GetterFirstH2,
	GetterSecondH2,
	GeterCanonicalLinkElement,
	GetterStatusCode,
	GetterTitleLength,
	GetterMetaDescriptionLength,
	GetterMetaKeywordsLength,
	GetterFirstH1Length,
	GetterSecondH1Length,
	GetterFirstH2Length,
	GetterSecondH2Length,
	GetterPageSizeBytes,
	GetterWordCount
};

using WebSiteAnalyseElementPtr = std::shared_ptr<WebsiteAnalyseElement>;

struct IWebsiteAnalyseElementHasher
{
	virtual size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept = 0;
};

template <int MemberGetterType>
struct WebsiteAnalyseElementHasher : public IWebsiteAnalyseElementHasher
{
	virtual size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept override
	{
		static_assert(MemberGetterType >= GetterUrl && MemberGetterType <= static_cast<int>(GetterWordCount), "Invalid MemberGetterType");

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;
		
		static std::map<int, std::function<size_t(WebSiteAnalyseElementPtr const&)>> s_hashFuncs
		{
			{ GetterUrl, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->url.toString().toStdString()); } },
			{ GetterContent, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->content.toStdString()); } },
			{ GetterMetaRefresh, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaRefresh.toStdString()); } },
			{ GetterMetaRobots, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaRobots.toStdString()); } },
			{ GetterRedirectedUrl, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->redirectedUrl.toStdString()); } },
			{ GetterServerResponse, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->serverResponse.toStdString()); } },
			{ GetterTitle, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->title.toStdString()); } },
			{ GetterMetaDescription, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaDescription.toStdString()); } },
			{ GetterMetaKeywords, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaKeywords.toStdString()); } },
			{ GetterFirstH1, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->firstH1.toStdString()); } },
			{ GetterSecondH1, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->secondH1.toStdString()); } },
			{ GetterFirstH2, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->firstH2.toStdString()); } },
			{ GetterSecondH2, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->secondH2.toStdString()); } },
			{ GeterCanonicalLinkElement, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->canonicalLinkElement.toStdString()); } },
			{ GetterStatusCode, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->statusCode); } },
			{ GetterTitleLength, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->titleLength); } },
			{ GetterMetaDescriptionLength, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->metaDescriptionLength); } },
			{ GetterMetaKeywordsLength, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->metaKeywordsLength); } },
			{ GetterFirstH1Length, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->firstH1Length); } },
			{ GetterSecondH1Length, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->secondH1Length); } },
			{ GetterFirstH2Length, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->firstH2Length); } },
			{ GetterSecondH2Length, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->secondH2Length); } },
			{ GetterPageSizeBytes, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->pageSizeBytes); } },
			{ GetterWordCount, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->wordCount); } }
		};

		return s_hashFuncs[MemberGetterType](websiteAnalyseElement);
	}
};

using WebsiteAnalyseElementHasherUrl = WebsiteAnalyseElementHasher<GetterUrl>;
using WebsiteAnalyseElementHasherContent = WebsiteAnalyseElementHasher<GetterContent>;
using WebsiteAnalyseElementHasherMetaRefresh = WebsiteAnalyseElementHasher<GetterMetaRefresh>;
using WebsiteAnalyseElementHasherMetaRobots = WebsiteAnalyseElementHasher<GetterMetaRobots>;
using WebsiteAnalyseElementHasherRedirectedUrl = WebsiteAnalyseElementHasher<GetterRedirectedUrl>;
using WebsiteAnalyseElementHasherServerResponse = WebsiteAnalyseElementHasher<GetterServerResponse>;
using WebsiteAnalyseElementHasherTitle = WebsiteAnalyseElementHasher<GetterTitle>;
using WebsiteAnalyseElementHasherMetaDescription = WebsiteAnalyseElementHasher<GetterMetaDescription>;
using WebsiteAnalyseElementHasherMetaKeywords = WebsiteAnalyseElementHasher<GetterMetaKeywords>;
using WebsiteAnalyseElementHasherFirstH1 = WebsiteAnalyseElementHasher<GetterFirstH1>;
using WebsiteAnalyseElementHasherSecondH1 = WebsiteAnalyseElementHasher<GetterSecondH1>;
using WebsiteAnalyseElementHasherFirstH2 = WebsiteAnalyseElementHasher<GetterFirstH2>;
using WebsiteAnalyseElementHasherSecondH2 = WebsiteAnalyseElementHasher<GetterSecondH2>;
using WebsiteAnalyseElementHasherCanonicalLinkElement = WebsiteAnalyseElementHasher<GeterCanonicalLinkElement>;
using WebsiteAnalyseElementHasherStatusCode = WebsiteAnalyseElementHasher<GetterStatusCode>;
using WebsiteAnalyseElementHasherTitleLength = WebsiteAnalyseElementHasher<GetterTitleLength>;
using WebsiteAnalyseElementHasherMetaDescriptionLength = WebsiteAnalyseElementHasher<GetterMetaDescriptionLength>;
using WebsiteAnalyseElementHasherMetaKeywordsLength = WebsiteAnalyseElementHasher<GetterMetaKeywordsLength>;
using WebsiteAnalyseElementHasherFirstH1Length = WebsiteAnalyseElementHasher<GetterFirstH1Length>;
using WebsiteAnalyseElementHasherSecondH1Length = WebsiteAnalyseElementHasher<GetterSecondH1Length>;
using WebsiteAnalyseElementHasherFirstH2Length = WebsiteAnalyseElementHasher<GetterFirstH2Length>;
using WebsiteAnalyseElementHasherSecondH2Length = WebsiteAnalyseElementHasher<GetterSecondH2Length>;
using WebsiteAnalyseElementHasherPageSizeBytes = WebsiteAnalyseElementHasher<GetterPageSizeBytes>;
using WebsiteAnalyseElementHasherWordCount = WebsiteAnalyseElementHasher<GetterWordCount>;

class UniversalWebsiteAnalyseElementHasher
{
public:
	UniversalWebsiteAnalyseElementHasher(std::shared_ptr<IWebsiteAnalyseElementHasher> const& hasher)
		: m_hasher(hasher)
	{
	}

	size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept
	{
		return (*m_hasher)(websiteAnalyseElement);
	}

private:
	std::shared_ptr<IWebsiteAnalyseElementHasher> m_hasher;
};

}
