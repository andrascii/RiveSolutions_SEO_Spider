#pragma once

#include "page_info.h"

namespace QuickieWebBot
{

struct IPageInfoHasher
{
	virtual size_t operator()(const PageInfoPtr& pageInfo) const noexcept = 0;
};

template <int ItemType>
struct PageInfoHasher : public IPageInfoHasher
{
	virtual size_t operator()(const PageInfoPtr& pageInfo) const noexcept override
	{
		static_assert(
			ItemType >= PageInfo::PageInfoItemTypeStart &&
			ItemType <= PageInfo::PageInfoItemTypeLast,
			"Invalid ItemType"
			);

		static boost::hash<std::string> s_stringHasher;
		static boost::hash<int> s_intHasher;

		static std::map<int, std::function<size_t(const PageInfoPtr&)>> s_hashFuncs
		{
			{ PageInfo::UrlItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::UrlItemType).toUrl().toDisplayString().toStdString()); } },
			{ PageInfo::ContentItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::ContentItemType).toString().toStdString()); } },
			{ PageInfo::MetaRefreshItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::MetaRefreshItemType).toString().toStdString()); } },
			{ PageInfo::MetaRobotsItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::MetaRobotsItemType).toString().toStdString()); } },
			{ PageInfo::RedirectedUrlItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::RedirectedUrlItemType).toString().toStdString()); } },
			{ PageInfo::ServerResponseItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::ServerResponseItemType).toString().toStdString()); } },
			{ PageInfo::TitleItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::TitleItemType).toString().toStdString()); } },
			{ PageInfo::MetaDescriptionItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::MetaDescriptionItemType).toString().toStdString()); } },
			{ PageInfo::MetaKeywordsItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::MetaKeywordsItemType).toString().toStdString()); } },
			{ PageInfo::FirstH1ItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::FirstH1ItemType).toString().toStdString()); } },
			{ PageInfo::SecondH1ItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::SecondH1ItemType).toString().toStdString()); } },
			{ PageInfo::FirstH2ItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::FirstH2ItemType).toString().toStdString()); } },
			{ PageInfo::SecondH2ItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::SecondH2ItemType).toString().toStdString()); } },
			{ PageInfo::CanonicalLinkElementItemType, [](const PageInfoPtr& el) { return s_stringHasher(el->itemValue(PageInfo::CanonicalLinkElementItemType).toString().toStdString()); } }
		};

		return s_hashFuncs[ItemType](pageInfo);
	}
};

using PageInfoHasherUrl = PageInfoHasher<PageInfo::UrlItemType>;
using PageInfoHasherContent = PageInfoHasher<PageInfo::ContentItemType>;
using PageInfoHasherMetaRefresh = PageInfoHasher<PageInfo::MetaRefreshItemType>;
using PageInfoHasherMetaRobots = PageInfoHasher<PageInfo::MetaRobotsItemType>;
using PageInfoHasherRedirectedUrl = PageInfoHasher<PageInfo::RedirectedUrlItemType>;
using PageInfoHasherServerResponse = PageInfoHasher<PageInfo::ServerResponseItemType>;
using PageInfoHasherTitle = PageInfoHasher<PageInfo::TitleItemType>;
using PageInfoHasherMetaDescription = PageInfoHasher<PageInfo::MetaDescriptionItemType>;
using PageInfoHasherMetaKeywords = PageInfoHasher<PageInfo::MetaKeywordsItemType>;
using PageInfoHasherFirstH1 = PageInfoHasher<PageInfo::FirstH1ItemType>;
using PageInfoHasherSecondH1 = PageInfoHasher<PageInfo::SecondH1ItemType>;
using PageInfoHasherFirstH2 = PageInfoHasher<PageInfo::FirstH2ItemType>;
using PageInfoHasherSecondH2 = PageInfoHasher<PageInfo::SecondH2ItemType>;
using PageInfoHasherCanonicalLinkElement = PageInfoHasher<PageInfo::CanonicalLinkElementItemType>;

}