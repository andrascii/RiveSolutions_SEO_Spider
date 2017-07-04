#pragma once

#include "website_analyse_element.h"
#include "table_model.h"

namespace QuickieWebBot
{

class ModelControllerData
	: public QObject
{
	Q_OBJECT
protected:
	using CrawlerStorageType = std::unordered_multiset<std::shared_ptr<WebsiteAnalyseElement>, UniversalWebsiteAnalyseElementHasher>;
	using CrawlerStorageTypePtr = std::shared_ptr<CrawlerStorageType>;

public:
	using GuiStorageType = QVector<std::shared_ptr<WebsiteAnalyseElement>>;
	using GuiStorageTypePtr = std::shared_ptr<GuiStorageType>;

	enum StorageType
	{
		InternalUrlStorageType,
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

	ModelControllerData(QObject* parent);

	bool isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, int storageType) const noexcept;
	void addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, int storageType) noexcept;

	GuiStorageType const* guiStorage(int type) const noexcept;
	GuiStorageType* guiStorage(int type) noexcept;

	Q_SIGNAL void rowAdded(int row, int storageType);

protected:
	CrawlerStorageType* crawlerStorage(int type) noexcept;
	CrawlerStorageType const* crawlerStorage(int type) const noexcept;
	
private:
	void checkStorageType(int type) const noexcept;

private:
	std::unordered_map<int, CrawlerStorageTypePtr> m_crawlerStorageMap;
	std::unordered_map<int, GuiStorageTypePtr> m_guiStorageMap;
};

}
