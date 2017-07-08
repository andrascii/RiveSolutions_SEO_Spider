#pragma once

namespace QuickieWebBot
{

struct PageInfo
{
	QUrl url;
	QString title;
	QString content;
	QString metaRefresh;
	QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QString redirectedUrl;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	QString canonicalLinkElement;
	int statusCode;
	int pageSizeBytes;
	int wordCount;
	size_t pageHash;
};

using PageInfoPtr = std::shared_ptr<PageInfo>;

struct IPageInfoHasher
{
	enum PageInfoMember
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
		PageSizeBytesMember,
		WordCountMember
	};

	virtual size_t operator()(const PageInfoPtr& pageInfo) const noexcept = 0;
};

template <int MemberType>
struct PageInfoHasher : public IPageInfoHasher
{
	virtual size_t operator()(const PageInfoPtr& pageInfo) const noexcept override
	{
		static_assert(MemberType >= UrlMember && MemberType <= static_cast<int>(WordCountMember), "Invalid MemberGetterType");

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;
		
		static std::map<int, std::function<size_t(const PageInfoPtr&)>> s_hashFuncs
		{
			{ UrlMember, [](PageInfoPtr const& el) { return s_stringHasher(el->url.toString().toStdString()); } },
			{ ContentMember, [](PageInfoPtr const& el) { return s_stringHasher(el->content.toStdString()); } },
			{ MetaRefreshMember, [](PageInfoPtr const& el) { return s_stringHasher(el->metaRefresh.toStdString()); } },
			{ MetaRobotsMember, [](PageInfoPtr const& el) { return s_stringHasher(el->metaRobots.toStdString()); } },
			{ RedirectedUrlMember, [](PageInfoPtr const& el) { return s_stringHasher(el->redirectedUrl.toStdString()); } },
			{ ServerResponseMember, [](PageInfoPtr const& el) { return s_stringHasher(el->serverResponse.toStdString()); } },
			{ TitleMember, [](PageInfoPtr const& el) { return s_stringHasher(el->title.toStdString()); } },
			{ MetaDescriptionMember, [](PageInfoPtr const& el) { return s_stringHasher(el->metaDescription.toStdString()); } },
			{ MetaKeywordsMember, [](PageInfoPtr const& el) { return s_stringHasher(el->metaKeywords.toStdString()); } },
			{ FirstH1Member, [](PageInfoPtr const& el) { return s_stringHasher(el->firstH1.toStdString()); } },
			{ SecondH1Member, [](PageInfoPtr const& el) { return s_stringHasher(el->secondH1.toStdString()); } },
			{ FirstH2Member, [](PageInfoPtr const& el) { return s_stringHasher(el->firstH2.toStdString()); } },
			{ SecondH2Member, [](PageInfoPtr const& el) { return s_stringHasher(el->secondH2.toStdString()); } },
			{ CanonicalLinkElementMember, [](PageInfoPtr const& el) { return s_stringHasher(el->canonicalLinkElement.toStdString()); } },
			{ StatusCodeMember, [](PageInfoPtr const& el) { return s_intHasher(el->statusCode); } },
			{ PageSizeBytesMember, [](PageInfoPtr const& el) { return s_intHasher(el->pageSizeBytes); } },
			{ WordCountMember, [](PageInfoPtr const& el) { return s_intHasher(el->wordCount); } }
		};

		return s_hashFuncs[MemberType](pageInfo);
	}
};

using PageInfoHasherUrl = PageInfoHasher<IPageInfoHasher::UrlMember>;
using PageInfoHasherContent = PageInfoHasher<IPageInfoHasher::ContentMember>;
using PageInfoHasherMetaRefresh = PageInfoHasher<IPageInfoHasher::MetaRefreshMember>;
using PageInfoHasherMetaRobots = PageInfoHasher<IPageInfoHasher::MetaRobotsMember>;
using PageInfoHasherRedirectedUrl = PageInfoHasher<IPageInfoHasher::RedirectedUrlMember>;
using PageInfoHasherServerResponse = PageInfoHasher<IPageInfoHasher::ServerResponseMember>;
using PageInfoHasherTitle = PageInfoHasher<IPageInfoHasher::TitleMember>;
using PageInfoHasherMetaDescription = PageInfoHasher<IPageInfoHasher::MetaDescriptionMember>;
using PageInfoHasherMetaKeywords = PageInfoHasher<IPageInfoHasher::MetaKeywordsMember>;
using PageInfoHasherFirstH1 = PageInfoHasher<IPageInfoHasher::FirstH1Member>;
using PageInfoHasherSecondH1 = PageInfoHasher<IPageInfoHasher::SecondH1Member>;
using PageInfoHasherFirstH2 = PageInfoHasher<IPageInfoHasher::FirstH2Member>;
using PageInfoHasherSecondH2 = PageInfoHasher<IPageInfoHasher::SecondH2Member>;
using PageInfoHasherCanonicalLinkElement = PageInfoHasher<IPageInfoHasher::CanonicalLinkElementMember>;
using PageInfoHasherStatusCode = PageInfoHasher<IPageInfoHasher::StatusCodeMember>;
using PageInfoHasherPageSizeBytes = PageInfoHasher<IPageInfoHasher::PageSizeBytesMember>;
using PageInfoHasherWordCount = PageInfoHasher<IPageInfoHasher::WordCountMember>;

class UniversalPageInfoHasher
{
public:
	UniversalPageInfoHasher(const std::shared_ptr<IPageInfoHasher>& hasher)
		: m_hasher(hasher)
	{
	}

	size_t operator()(const PageInfoPtr& pageInfo) const noexcept
	{
		return (*m_hasher)(pageInfo);
	}

private:
	std::shared_ptr<IPageInfoHasher> m_hasher;
};


Q_DECLARE_METATYPE(PageInfoPtr);

}
