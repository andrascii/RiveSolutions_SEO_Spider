#pragma once

#include "page_info.h"
#include "gridview_model.h"

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
		CrawledUrlStorageType,
		ExternalUrlStorageType,

		UpperCaseUrlStorageType,
		NonAsciiCharacterUrlStorageType,
		VeryLongUrlStorageType,

		EmptyTitleUrlStorageType,
		DuplicatedTitleUrlStorageType,
		VeryLongTitleUrlStorageType,
		VeryShortTitleUrlStorageType,
		DuplicatedH1TitleUrlStorageType,
		SeveralTitleUrlStorageType,

		EmptyMetaDescriptionUrlStorageType,
		DuplicatedMetaDescriptionUrlStorageType,
		VeryLongMetaDescriptionUrlStorageType,
		VeryShortMetaDescriptionUrlStorageType,
		SeveralMetaDescriptionUrlStorageType,

		EmptyMetaKeywordsUrlStorageType,
		DuplicatedMetaKeywordsUrlStorageType,
		SeveralMetaKeywordsUrlStorageType,

		MissingH1UrlStorageType,
		DuplicatedH1UrlStorageType,
		VeryLongH1UrlStorageType,
		SeveralH1UrlStorageType,

		MissingH2UrlStorageType,
		DuplicatedH2UrlStorageType,
		VeryLongH2UrlStorageType,
		SeveralH2UrlStorageType,

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
