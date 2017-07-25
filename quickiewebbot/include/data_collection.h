#pragma once

#include "page_info.h"
#include "grid_view_model.h"
#include "universal_page_info_hasher.h"

namespace QuickieWebBot
{

class DataCollection : public QObject
{
	Q_OBJECT

protected:
	using CrawlerStorageType = std::unordered_multiset<PageInfoPtr, UniversalPageInfoHasher>;
	using CrawlerStorageTypePtr = std::shared_ptr<CrawlerStorageType>;

public:
	using GuiStorageType = QVector<PageInfoPtr>;
	using GuiStorageTypePtr = std::shared_ptr<GuiStorageType>;

	enum StorageType
	{
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
		EmptyTitleUrlStorageType,
		DuplicatedTitleUrlStorageType,
		VeryLongTitleUrlStorageType,
		VeryShortTitleUrlStorageType,
		DuplicatedH1TitleUrlStorageType,
		SeveralTitleUrlStorageType,

		//
		// Meta description problems
		//
		EmptyMetaDescriptionUrlStorageType,
		DuplicatedMetaDescriptionUrlStorageType,
		VeryLongMetaDescriptionUrlStorageType,
		VeryShortMetaDescriptionUrlStorageType,
		SeveralMetaDescriptionUrlStorageType,

		//
		// Meta keywords problems
		//
		EmptyMetaKeywordsUrlStorageType,
		DuplicatedMetaKeywordsUrlStorageType,
		SeveralMetaKeywordsUrlStorageType,

		//
		// H1 problems
		//
		MissingH1UrlStorageType,
		DuplicatedH1UrlStorageType,
		VeryLongH1UrlStorageType,
		SeveralH1UrlStorageType,

		//
		// H2 problems
		//
		MissingH2UrlStorageType,
		DuplicatedH2UrlStorageType,
		VeryLongH2UrlStorageType,
		SeveralH2UrlStorageType,

		//
		// Images problems
		//
		Over100kbImageStorageType,
		MissingAltTextImageStorageType,
		VeryLongAltTextImageStorageType
	};

	DataCollection(QObject* parent);

	bool isPageInfoExists(const PageInfoPtr& pageInfo, int storageType) const noexcept;
	void addPageInfo(const PageInfoPtr& pageInfo, int storageType) noexcept;

	GuiStorageType const* guiStorage(int type) const noexcept;
	GuiStorageType* guiStorage(int type) noexcept;

	Q_SIGNAL void pageInfoAdded(int row, int storageType);

protected:
	CrawlerStorageType* crawlerStorage(int type) noexcept;
	const CrawlerStorageType* crawlerStorage(int type) const noexcept;
	
private:
	void checkStorageType(int type) const noexcept;

private:
	std::unordered_map<int, CrawlerStorageTypePtr> m_crawlerStorageMap;
	std::unordered_map<int, GuiStorageTypePtr> m_guiStorageMap;
};

}
