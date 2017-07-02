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




template <int MemberGetterType>
struct WebsiteAnalyseElementHasher
{

	using WebSiteAnalyseElementPtr = std::shared_ptr<WebsiteAnalyseElement>;
	enum WebSiteAnalyseElementMemberGetter {
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


	size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept
	{
		static_assert(MemberGetterType >= GetterUrl && MemberGetterType <= static_cast<int>(GetterWordCount), "Invalid MemberGetterType");
		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;
		
		static std::map<WebSiteAnalyseElementMemberGetter, std::function<size_t(WebSiteAnalyseElementPtr const&)>> s_hashFuncs{
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

		return s_hashFuncs[static_cast<WebSiteAnalyseElementMemberGetter>(MemberGetterType)](websiteAnalyseElement);
	}
};


using WebsiteAnalyseElementHasherUrl = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterUrl>;
using WebsiteAnalyseElementHasherContent = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterContent>;
using WebsiteAnalyseElementHasherMetaRefresh = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaRefresh>;
using WebsiteAnalyseElementHasherMetaRobots = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaRobots>;
using WebsiteAnalyseElementHasherRedirectedUrl = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterRedirectedUrl>;
using WebsiteAnalyseElementHasherServerResponse = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterServerResponse>;
using WebsiteAnalyseElementHasherTitle = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterTitle>;
using WebsiteAnalyseElementHasherMetaDescription = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaDescription>;
using WebsiteAnalyseElementHasherMetaKeywords = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaKeywords>;
using WebsiteAnalyseElementHasherFirstH1 = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterFirstH1>;
using WebsiteAnalyseElementHasherSecondH1 = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterSecondH1>;
using WebsiteAnalyseElementHasherFirstH2 = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterFirstH2>;
using WebsiteAnalyseElementHasherSecondH2 = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterSecondH2>;
using WebsiteAnalyseElementHasherCanonicalLinkElement = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GeterCanonicalLinkElement>;
using WebsiteAnalyseElementHasherStatusCode = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterStatusCode>;
using WebsiteAnalyseElementHasherTitleLength = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterTitleLength>;
using WebsiteAnalyseElementHasherMetaDescriptionLength = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaDescriptionLength>;
using WebsiteAnalyseElementHasherMetaKeywordsLength = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterMetaKeywordsLength>;
using WebsiteAnalyseElementHasherFirstH1Length = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterFirstH1Length>;
using WebsiteAnalyseElementHasherSecondH1Length = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterSecondH1Length>;
using WebsiteAnalyseElementHasherFirstH2Length = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterFirstH2Length>;
using WebsiteAnalyseElementHasherSecondH2Length = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterSecondH2Length>;
using WebsiteAnalyseElementHasherPageSizeBytes = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterPageSizeBytes>;
using WebsiteAnalyseElementHasherWordCount = WebsiteAnalyseElementHasher<WebsiteAnalyseElementHasher<0>::GetterWordCount>;


}
