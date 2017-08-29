#pragma once

#include "page_raw.h"

namespace WebCrawler
{

struct IPageRawHasher
{
	enum HasherType
	{
		UrlItemType,
		ContentItemType,
		MetaRefreshItemType,
		MetaRobotsItemType,
		RedirectedUrlItemType,
		ServerResponseItemType,
		TitleItemType,
		MetaDescriptionItemType,
		MetaKeywordsItemType,
		FirstH1ItemType,
		SecondH1ItemType,
		FirstH2ItemType,
		SecondH2ItemType,
		CanonicalLinkElementItemType
	};

	virtual size_t operator()(const PageRawPtr& pageRaw) const noexcept = 0;
};

template <int ItemType>
struct PageRawHasher : public IPageRawHasher
{
	virtual size_t operator()(const PageRawPtr& pageRaw) const noexcept override
	{
		static_assert(
			ItemType >= IPageRawHasher::UrlItemType &&
			ItemType <= IPageRawHasher::CanonicalLinkElementItemType,
			"Invalid ItemType"
			);

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;

		static std::map<int, std::function<size_t(const PageRawPtr&)>> s_hashFuncs
		{
			{ IPageRawHasher::UrlItemType, [](const PageRawPtr& el) { return s_stringHasher(el->url.toDisplayString().toStdString()); } },
			{ IPageRawHasher::ContentItemType, [](const PageRawPtr& el) { return s_stringHasher(el->content.toStdString()); } },
			{ IPageRawHasher::MetaRefreshItemType, [](const PageRawPtr& el) { return s_stringHasher(el->metaRefresh.toStdString()); } },
			{ IPageRawHasher::MetaRobotsItemType, [](const PageRawPtr& el) { return s_stringHasher(el->metaRobots.toStdString()); } },
			{ IPageRawHasher::RedirectedUrlItemType, [](const PageRawPtr& el) { return s_stringHasher(el->redirectedUrl.toStdString()); } },
			{ IPageRawHasher::ServerResponseItemType, [](const PageRawPtr& el) { return s_stringHasher(el->serverResponse.toStdString()); } },
			{ IPageRawHasher::TitleItemType, [](const PageRawPtr& el) { return s_stringHasher(el->title.toStdString()); } },
			{ IPageRawHasher::MetaDescriptionItemType, [](const PageRawPtr& el) { return s_stringHasher(el->metaDescription.toStdString()); } },
			{ IPageRawHasher::MetaKeywordsItemType, [](const PageRawPtr& el) { return s_stringHasher(el->metaKeywords.toStdString()); } },
			{ IPageRawHasher::FirstH1ItemType, [](const PageRawPtr& el) { return s_stringHasher(el->firstH1.toStdString()); } },
			{ IPageRawHasher::SecondH1ItemType, [](const PageRawPtr& el) { return s_stringHasher(el->secondH1.toStdString()); } },
			{ IPageRawHasher::FirstH2ItemType, [](const PageRawPtr& el) { return s_stringHasher(el->firstH2.toStdString()); } },
			{ IPageRawHasher::SecondH2ItemType, [](const PageRawPtr& el) { return s_stringHasher(el->secondH2.toStdString()); } },
			{ IPageRawHasher::CanonicalLinkElementItemType, [](const PageRawPtr& el) { return s_stringHasher(el->canonicalLinkElement.toStdString()); } }
		};

		return s_hashFuncs[ItemType](pageRaw);
	}
};

using PageRawHasherUrl = PageRawHasher<IPageRawHasher::UrlItemType>;
using PageRawHasherContent = PageRawHasher<IPageRawHasher::ContentItemType>;
using PageRawHasherMetaRefresh = PageRawHasher<IPageRawHasher::MetaRefreshItemType>;
using PageRawHasherMetaRobots = PageRawHasher<IPageRawHasher::MetaRobotsItemType>;
using PageRawHasherRedirectedUrl = PageRawHasher<IPageRawHasher::RedirectedUrlItemType>;
using PageRawHasherServerResponse = PageRawHasher<IPageRawHasher::ServerResponseItemType>;
using PageRawHasherTitle = PageRawHasher<IPageRawHasher::TitleItemType>;
using PageRawHasherMetaDescription = PageRawHasher<IPageRawHasher::MetaDescriptionItemType>;
using PageRawHasherMetaKeywords = PageRawHasher<IPageRawHasher::MetaKeywordsItemType>;
using PageRawHasherFirstH1 = PageRawHasher<IPageRawHasher::FirstH1ItemType>;
using PageRawHasherSecondH1 = PageRawHasher<IPageRawHasher::SecondH1ItemType>;
using PageRawHasherFirstH2 = PageRawHasher<IPageRawHasher::FirstH2ItemType>;
using PageRawHasherSecondH2 = PageRawHasher<IPageRawHasher::SecondH2ItemType>;
using PageRawHasherCanonicalLinkElement = PageRawHasher<IPageRawHasher::CanonicalLinkElementItemType>;

}