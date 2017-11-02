#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

struct IParsedPageHasher
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

	virtual size_t operator()(const ParsedPagePtr& pageRaw) const noexcept = 0;
};

template <int ItemType>
struct ParsedPageHasher : public IParsedPageHasher
{
	virtual size_t operator()(const ParsedPagePtr& pageRaw) const noexcept override
	{
		static_assert(
			ItemType >= IParsedPageHasher::UrlItemType &&
			ItemType <= IParsedPageHasher::CanonicalLinkElementItemType,
			"Invalid ItemType"
			);

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;

		static std::map<int, std::function<size_t(const ParsedPagePtr&)>> s_hashFuncs
		{
			{ IParsedPageHasher::UrlItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->url.toDisplayString().toStdString()); } },
			{ IParsedPageHasher::ContentItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->contentType.toStdString()); } },
			{ IParsedPageHasher::MetaRefreshItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->metaRefresh.toStdString()); } },
			{ IParsedPageHasher::MetaRobotsItemType, [](const ParsedPagePtr& el) { return s_intHasher(el->metaRobotsFlags); } },
			{ IParsedPageHasher::RedirectedUrlItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->redirectedUrl.toDisplayString().toStdString()); } },
			{ IParsedPageHasher::ServerResponseItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->serverResponse.toStdString()); } },
			{ IParsedPageHasher::TitleItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->title.toStdString()); } },
			{ IParsedPageHasher::MetaDescriptionItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->metaDescription.toStdString()); } },
			{ IParsedPageHasher::MetaKeywordsItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->metaKeywords.toStdString()); } },
			{ IParsedPageHasher::FirstH1ItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->firstH1.toStdString()); } },
			{ IParsedPageHasher::SecondH1ItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->secondH1.toStdString()); } },
			{ IParsedPageHasher::FirstH2ItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->firstH2.toStdString()); } },
			{ IParsedPageHasher::SecondH2ItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->secondH2.toStdString()); } },
			{ IParsedPageHasher::CanonicalLinkElementItemType, [](const ParsedPagePtr& el) { return s_stringHasher(el->canonicalLinkElement.toStdString()); } }
		};

		return s_hashFuncs[ItemType](pageRaw);
	}
};

using ParsedPageHasherUrl = ParsedPageHasher<IParsedPageHasher::UrlItemType>;
using ParsedPageHasherContent = ParsedPageHasher<IParsedPageHasher::ContentItemType>;
using ParsedPageHasherMetaRefresh = ParsedPageHasher<IParsedPageHasher::MetaRefreshItemType>;
using ParsedPageHasherMetaRobots = ParsedPageHasher<IParsedPageHasher::MetaRobotsItemType>;
using ParsedPageHasherRedirectedUrl = ParsedPageHasher<IParsedPageHasher::RedirectedUrlItemType>;
using ParsedPageHasherServerResponse = ParsedPageHasher<IParsedPageHasher::ServerResponseItemType>;
using ParsedPageHasherTitle = ParsedPageHasher<IParsedPageHasher::TitleItemType>;
using ParsedPageHasherMetaDescription = ParsedPageHasher<IParsedPageHasher::MetaDescriptionItemType>;
using ParsedPageHasherMetaKeywords = ParsedPageHasher<IParsedPageHasher::MetaKeywordsItemType>;
using ParsedPageHasherFirstH1 = ParsedPageHasher<IParsedPageHasher::FirstH1ItemType>;
using ParsedPageHasherSecondH1 = ParsedPageHasher<IParsedPageHasher::SecondH1ItemType>;
using ParsedPageHasherFirstH2 = ParsedPageHasher<IParsedPageHasher::FirstH2ItemType>;
using ParsedPageHasherSecondH2 = ParsedPageHasher<IParsedPageHasher::SecondH2ItemType>;
using ParsedPageHasherCanonicalLinkElement = ParsedPageHasher<IParsedPageHasher::CanonicalLinkElementItemType>;

}