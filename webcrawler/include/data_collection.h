#pragma once

#include "page_raw.h"
#include "page_raw_hasher_proxy.h"
#include "page_raw_comparator.h"

namespace WebCrawler
{

class GuiStorage;

class DataCollection : public QObject
{
	Q_OBJECT

protected:
	using CrawlerStorageType = std::unordered_multiset<PageRawPtr, PageRawHasherProxy, PageRawComparatorProxy>;
	using CrawlerStorageTypePtr = std::shared_ptr<CrawlerStorageType>;

public:
	enum StorageType
	{
		BeginEnumStorageType,
		// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

		//
		// Statistic data
		//
		CrawledUrlStorageType,
		ExternalUrlStorageType,

		//
		// Url problems
		//
		UpperCaseUrlStorageType,
		NonAsciiCharacterUrlStorageType,
		VeryLongUrlStorageType,

		//
		// Title problems
		//
		AllTitlesUrlStorageType,
		EmptyTitleUrlStorageType,
		DuplicatedTitleUrlStorageType,
		VeryLongTitleUrlStorageType,
		VeryShortTitleUrlStorageType,
		DuplicatedH1TitleUrlStorageType,
		SeveralTitleUrlStorageType,

		//
		// Meta description problems
		//
		AllMetaDescriptionsUrlStorageType,
		EmptyMetaDescriptionUrlStorageType,
		DuplicatedMetaDescriptionUrlStorageType,
		VeryLongMetaDescriptionUrlStorageType,
		VeryShortMetaDescriptionUrlStorageType,
		SeveralMetaDescriptionUrlStorageType,

		//
		// Meta keywords problems
		//
		AllMetaKeywordsUrlStorageType,
		EmptyMetaKeywordsUrlStorageType,
		DuplicatedMetaKeywordsUrlStorageType,
		SeveralMetaKeywordsUrlStorageType,

		//
		// H1 problems
		//
		AllH1UrlStorageType,
		MissingH1UrlStorageType,
		DuplicatedH1UrlStorageType,
		VeryLongH1UrlStorageType,
		SeveralH1UrlStorageType,

		//
		// H2 problems
		//
		AllH2UrlStorageType,
		MissingH2UrlStorageType,
		DuplicatedH2UrlStorageType,
		VeryLongH2UrlStorageType,
		SeveralH2UrlStorageType,

		//
		// Images problems
		//
		Over100kbImageStorageType,
		MissingAltTextImageStorageType,
		VeryLongAltTextImageStorageType,

		//
		// Statuses
		//
		Status404StorageType,


		//
		// Resources
		//
		PendingResourcesStorageType,
		HtmlResourcesStorageType,
		ImageResourcesStorageType,
		JavaScriptResourcesStorageType,
		StyleSheetResourcesStorageType,
		FlashResourcesStorageType,
		VideoResourcesStorageType,
		OtherResourcesStorageType,

		//
		// ExternalResources
		//
		ExternalHtmlResourcesStorageType,
		ExternalImageResourcesStorageType,
		ExternalJavaScriptResourcesStorageType,
		ExternalStyleSheetResourcesStorageType,
		ExternalFlashResourcesStorageType,
		ExternalVideoResourcesStorageType,
		ExternalOtherResourcesStorageType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumStorageType
	};

	DataCollection(QObject* parent);
	~DataCollection();

	bool isPageRawExists(const PageRawPtr& pageRaw, StorageType type) const noexcept;
	void addPageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept;
	PageRawPtr removePageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept;
	const PageRawPtr pageRaw(const PageRawPtr& pageRaw, StorageType type) const noexcept;

	GuiStorage* guiStorage() const noexcept;

	Q_SIGNAL void pageRawAdded(PageRawPtr pageRaw, int type);

protected:
	CrawlerStorageTypePtr& crawlerStorage(StorageType type) noexcept;
	const CrawlerStorageTypePtr& crawlerStorage(StorageType type) const noexcept;
	
private:
	void checkStorageType(StorageType type) const noexcept;
	void initializeStorages();

private:
	std::unordered_map<int, CrawlerStorageTypePtr> m_crawlerStorageMap;
	GuiStorage* m_guiStorage;
};

}
