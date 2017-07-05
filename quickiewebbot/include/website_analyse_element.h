#pragma once

namespace QuickieWebBot
{

struct WebSiteAnalyseElement
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

using WebSiteAnalyseElementPtr = std::shared_ptr<WebSiteAnalyseElement>;

struct IWebsiteAnalyseElementHasher
{
	enum WebSiteAnalyseElementMember
	{
		UrlMember,
		ContentMember,
		MetaRefreshMember,
		MetaRobotsMember,
		RedirectedUrlMember,
		ServerResponseMember,
		TitleMember,
		MetaDescriptionMember,
		MetaKeywordsMember,
		FirstH1Member,
		SecondH1Member,
		FirstH2Member,
		SecondH2Member,
		CanonicalLinkElementMember,
		StatusCodeMember,
		TitleLengthMember,
		MetaDescriptionLengthMember,
		MetaKeywordsLengthMember,
		FirstH1LengthMember,
		SecondH1LengthMember,
		FirstH2LengthMember,
		SecondH2LengthMember,
		PageSizeBytesMember,
		WordCountMember
	};

	virtual size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept = 0;
};

template <int MemberType>
struct WebSiteAnalyseElementHasher : public IWebsiteAnalyseElementHasher
{
	virtual size_t operator()(WebSiteAnalyseElementPtr const& websiteAnalyseElement) const noexcept override
	{
		static_assert(MemberType >= UrlMember && MemberType <= static_cast<int>(WordCountMember), "Invalid MemberGetterType");

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;
		
		static std::map<int, std::function<size_t(WebSiteAnalyseElementPtr const&)>> s_hashFuncs
		{
			{ UrlMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->url.toString().toStdString()); } },
			{ ContentMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->content.toStdString()); } },
			{ MetaRefreshMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaRefresh.toStdString()); } },
			{ MetaRobotsMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaRobots.toStdString()); } },
			{ RedirectedUrlMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->redirectedUrl.toStdString()); } },
			{ ServerResponseMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->serverResponse.toStdString()); } },
			{ TitleMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->title.toStdString()); } },
			{ MetaDescriptionMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaDescription.toStdString()); } },
			{ MetaKeywordsMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->metaKeywords.toStdString()); } },
			{ FirstH1Member, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->firstH1.toStdString()); } },
			{ SecondH1Member, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->secondH1.toStdString()); } },
			{ FirstH2Member, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->firstH2.toStdString()); } },
			{ SecondH2Member, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->secondH2.toStdString()); } },
			{ CanonicalLinkElementMember, [](WebSiteAnalyseElementPtr const& el) { return s_stringHasher(el->canonicalLinkElement.toStdString()); } },
			{ StatusCodeMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->statusCode); } },
			{ TitleLengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->titleLength); } },
			{ MetaDescriptionLengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->metaDescriptionLength); } },
			{ MetaKeywordsLengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->metaKeywordsLength); } },
			{ FirstH1LengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->firstH1Length); } },
			{ SecondH1LengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->secondH1Length); } },
			{ FirstH2LengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->firstH2Length); } },
			{ SecondH2LengthMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->secondH2Length); } },
			{ PageSizeBytesMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->pageSizeBytes); } },
			{ WordCountMember, [](WebSiteAnalyseElementPtr const& el) { return s_intHasher(el->wordCount); } }
		};

		return s_hashFuncs[MemberType](websiteAnalyseElement);
	}
};

using WebsiteAnalyseElementHasherUrl = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::UrlMember>;
using WebsiteAnalyseElementHasherContent = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::ContentMember>;
using WebsiteAnalyseElementHasherMetaRefresh = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaRefreshMember>;
using WebsiteAnalyseElementHasherMetaRobots = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaRobotsMember>;
using WebsiteAnalyseElementHasherRedirectedUrl = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::RedirectedUrlMember>;
using WebsiteAnalyseElementHasherServerResponse = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::ServerResponseMember>;
using WebsiteAnalyseElementHasherTitle = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::TitleMember>;
using WebsiteAnalyseElementHasherMetaDescription = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaDescriptionMember>;
using WebsiteAnalyseElementHasherMetaKeywords = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaKeywordsMember>;
using WebsiteAnalyseElementHasherFirstH1 = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::FirstH1Member>;
using WebsiteAnalyseElementHasherSecondH1 = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::SecondH1Member>;
using WebsiteAnalyseElementHasherFirstH2 = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::FirstH2Member>;
using WebsiteAnalyseElementHasherSecondH2 = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::SecondH2Member>;
using WebsiteAnalyseElementHasherCanonicalLinkElement = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::CanonicalLinkElementMember>;
using WebsiteAnalyseElementHasherStatusCode = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::StatusCodeMember>;
using WebsiteAnalyseElementHasherTitleLength = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::TitleLengthMember>;
using WebsiteAnalyseElementHasherMetaDescriptionLength = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaDescriptionLengthMember>;
using WebsiteAnalyseElementHasherMetaKeywordsLength = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::MetaKeywordsLengthMember>;
using WebsiteAnalyseElementHasherFirstH1Length = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::FirstH1LengthMember>;
using WebsiteAnalyseElementHasherSecondH1Length = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::SecondH1LengthMember>;
using WebsiteAnalyseElementHasherFirstH2Length = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::FirstH2LengthMember>;
using WebsiteAnalyseElementHasherSecondH2Length = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::SecondH2LengthMember>;
using WebsiteAnalyseElementHasherPageSizeBytes = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::PageSizeBytesMember>;
using WebsiteAnalyseElementHasherWordCount = WebSiteAnalyseElementHasher<IWebsiteAnalyseElementHasher::WordCountMember>;

class UniversalWebSiteAnalyseElementHasher
{
public:
	UniversalWebSiteAnalyseElementHasher(std::shared_ptr<IWebsiteAnalyseElementHasher> const& hasher)
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
