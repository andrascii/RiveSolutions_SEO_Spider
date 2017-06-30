#pragma once

#include "website_analyse_element.h"
#include "table_model.h"

namespace QuickieWebBot
{

class ModelControllerData
{
private:
	using CrawlerStorageType = std::unordered_set<std::shared_ptr<WebsiteAnalyseElement>, WebsiteAnalyseElementHasher>;
	using GuiStorageType = std::vector<std::shared_ptr<WebsiteAnalyseElement>>;

public:
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

	bool isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, StorageType type) const noexcept;
	void addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement, StorageType type) noexcept;

	CrawlerStorageType* crawlerStorage(StorageType type) noexcept;
	CrawlerStorageType const* crawlerStorage(StorageType type) const noexcept;

	GuiStorageType* guiStorage(StorageType type) noexcept;
	GuiStorageType const* guiStorage(StorageType type) const noexcept;

private:
	void checkStorageType(StorageType type) const noexcept;

private:
	template <typename StorageType>
	struct Storage
	{
		// internal crawler url queues
		StorageType internalUrlStorage;
		StorageType crawledUrlStorage;
		StorageType externalUrlStorage;

		// urls
		StorageType upperCaseUrlStorage;
		StorageType nonAsciiCharacterUrlStorage;
		StorageType veryLongUrlStorage;

		// titles
		StorageType emptyTitleUrlStorage;
		StorageType duplicatedTitleUrlStorage;
		StorageType veryLongTitleUrlStorage;
		StorageType veryShortTitleUrlStorage;
		StorageType duplicatedH1TitleUrlStorage;
		StorageType severalTitleUrlStorage;

		// meta descriptions
		StorageType emptyMetaDescriptionUrlStorage;
		StorageType duplicatedMetaDescriptionUrlStorage;
		StorageType veryLongMetaDescriptionUrlStorage;
		StorageType veryShortMetaDescriptionUrlStorage;
		StorageType severalMetaDescriptionUrlStorage;

		// meta keywords
		StorageType emptyMetaKeywordsUrlStorage;
		StorageType duplicatedMetaKeywordsUrlStorage;
		StorageType severalMetaKeywordsUrlStorage;

		// H1
		StorageType missingH1UrlStorage;
		StorageType duplicatedH1UrlStorage;
		StorageType veryLongH1UrlStorage;
		StorageType severalH1UrlStorage;

		// H2
		StorageType missingH2UrlStorage;
		StorageType duplicatedH2UrlStorage;
		StorageType veryLongH2UrlStorage;
		StorageType severalH2UrlStorage;

		// images
		StorageType over100kbImageStorage;
		StorageType missingAltTextImageStorage;
		StorageType veryLongAltTextImageStorage;
	};

private:
	Storage<CrawlerStorageType> m_crawlerStorage;
	Storage<GuiStorageType> m_guiStorage;
};

}
