#pragma once

#include "page_raw.h"
#include "universal_page_raw_hasher.h"

namespace WebCrawler
{

//class StorageAdaptorFactory;

class DataCollection : public QObject
{
	Q_OBJECT

protected:
	using CrawlerStorageType = std::unordered_multiset<PageRawPtr, UniversalPageRawHasher>;
	using CrawlerStorageTypePtr = std::shared_ptr<CrawlerStorageType>;

public:
	using GuiStorageType = QVector<PageRawPtr>;
	using GuiStorageTypePtr = std::shared_ptr<GuiStorageType>;

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
		HtmlPendingResourcesStorageType,
		HtmlResourcesStorageType,
		ImageResourcesStorageType,
		JavaScriptResourcesStorageType,
		StyleSheetResourcesStorageType,

		// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
		EndEnumStorageType
	};

	DataCollection(QObject* parent);

	bool isPageRawExists(const PageRawPtr& pageRaw, StorageType type) const noexcept;
	void addPageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept;
	PageRawPtr removePageRaw(const PageRawPtr& pageRaw, StorageType type) noexcept;
	const PageRawPtr& pageRaw(const PageRawPtr& pageRaw, StorageType type) const noexcept;

	const GuiStorageTypePtr& guiStorage(StorageType type) const noexcept;
	GuiStorageTypePtr& guiStorage(StorageType type) noexcept;

	Q_SIGNAL void pageRawAdded(int row, int storageType);

protected:
	CrawlerStorageTypePtr& crawlerStorage(StorageType type) noexcept;
	const CrawlerStorageTypePtr& crawlerStorage(StorageType type) const noexcept;
	
private:
	void checkStorageType(StorageType type) const noexcept;

private:
	std::unordered_map<int, CrawlerStorageTypePtr> m_crawlerStorageMap;
	std::unordered_map<int, GuiStorageTypePtr> m_guiStorageMap;
};

}
