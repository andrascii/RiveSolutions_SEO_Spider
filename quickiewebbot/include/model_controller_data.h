#pragma once

#include "website_analyse_element.h"
#include "table_model.h"

namespace QuickieWebBot
{

class ModelControllerData
{
private:
	template <class THasher>
	using CrawlerStorageType = std::unordered_multiset<std::shared_ptr<WebsiteAnalyseElement>, THasher>;

	using CrawlerStorageTypeUrlHashed = CrawlerStorageType<WebsiteAnalyseElementHasherUrl>;
	using CrawlerStorageTypeTitleHashed = CrawlerStorageType<WebsiteAnalyseElementHasherTitle>;
	using CrawlerStorageTypeMetaDescriptionHashed = CrawlerStorageType<WebsiteAnalyseElementHasherMetaDescription>;
	using CrawlerStorageTypeMetaKeywordsHashed = CrawlerStorageType<WebsiteAnalyseElementHasherMetaKeywords>;
	using CrawlerStorageTypeH1Hashed = CrawlerStorageType<WebsiteAnalyseElementHasherFirstH1>;
	using CrawlerStorageTypeH2Hashed = CrawlerStorageType<WebsiteAnalyseElementHasherFirstH2>;
	
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

	

	template <int StorageT>
	bool isElementExists(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement) const noexcept
	{
		checkStorageType(static_cast<StorageType>(StorageT));
		auto const* pQueue = crawlerStorage<StorageT>();
		return pQueue->find(websiteAnalysElement) != std::end(*pQueue);
	}

	template <int StorageT>
	void addElement(std::shared_ptr<WebsiteAnalyseElement> const& websiteAnalysElement) noexcept
	{
		if (isElementExists<StorageT>(websiteAnalysElement))
		{
			return;
		}

		crawlerStorage<StorageT>()->insert(websiteAnalysElement);
		guiStorage(StorageT)->push_back(websiteAnalysElement);
	}

	GuiStorageType* guiStorage(StorageType type) noexcept;
	GuiStorageType const* guiStorage(StorageType type) const noexcept;


protected:
	template <int StorageT, typename T,
		typename = std::enable_if<StorageT >= InternalUrlStorageType && StorageT <= VeryLongUrlStorageType
		|| StorageT >= Over100kbImageStorageType && StorageT <= VeryLongAltTextImageStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeUrlHashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <int StorageT, typename T, typename T2,
		typename = std::enable_if<StorageT >= EmptyTitleUrlStorageType && StorageT <= SeveralTitleUrlStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeTitleHashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <int StorageT, typename T, typename T2, typename T3,
		typename = std::enable_if<StorageT >= EmptyMetaDescriptionUrlStorageType && StorageT <= SeveralMetaDescriptionUrlStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeMetaDescriptionHashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <int StorageT, typename T, typename T2, typename T3, typename T4,
		typename = std::enable_if<StorageT >= EmptyMetaKeywordsUrlStorageType && StorageT <= SeveralMetaKeywordsUrlStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeMetaKeywordsHashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <int StorageT, typename T, typename T2, typename T3, typename T4, typename T5,
		typename = std::enable_if<StorageT >= MissingH1UrlStorageType && StorageT <= SeveralH1UrlStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeH1Hashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <int StorageT, typename T, typename T2, typename T3, typename T4, typename T5, typename T6,
		typename = std::enable_if<StorageT >= MissingH2UrlStorageType && StorageT <= SeveralH2UrlStorageType>>
		T const* crawlerStorage() const noexcept
	{
		return static_cast<CrawlerStorageTypeH2Hashed*>(m_crawlerStorage.allStorages[StorageT]);
	}

	template <typename T>
	T* crawlerStorage(StorageType type) noexcept
	{
		auto const* pQueue = const_cast<ModelControllerData const * const>(this)->crawlerStorage(type);
		return const_cast<std::remove_const_t<decltype(pQueue)>*>(pQueue);
	}


private:
	void checkStorageType(StorageType type) const noexcept;


private:
	template <typename StorageTypeUrl, typename StorageTypeTitle, typename StorageTypeMetaDescription, 
		typename StorageTypeMetaKeywords, typename StorageTypeH1, typename StorageTypeH2>
	struct Storage
	{
		Storage()
		{
			allStorages = {
				{ InternalUrlStorageType, &internalUrlStorage },
				{ CrawledUrlStorageType, &crawledUrlStorage },
				{ ExternalUrlStorageType, &externalUrlStorage },

				{ UpperCaseUrlStorageType, &upperCaseUrlStorage },
				{ NonAsciiCharacterUrlStorageType, &nonAsciiCharacterUrlStorage },
				{ VeryLongUrlStorageType, &veryLongUrlStorage },

				{ EmptyTitleUrlStorageType, &emptyTitleUrlStorage},
				{ DuplicatedTitleUrlStorageType, &duplicatedTitleUrlStorage },
				{ VeryLongTitleUrlStorageType, &veryLongTitleUrlStorage },
				{ VeryShortTitleUrlStorageType, &veryShortTitleUrlStorage }, 
				{ DuplicatedH1TitleUrlStorageType, &duplicatedH1TitleUrlStorage },
				{ SeveralTitleUrlStorageType, &severalTitleUrlStorage },

				{ EmptyMetaDescriptionUrlStorageType, &emptyMetaDescriptionUrlStorage },
				{ DuplicatedMetaDescriptionUrlStorageType, &duplicatedMetaDescriptionUrlStorage },
				{ VeryLongMetaDescriptionUrlStorageType, &veryLongMetaDescriptionUrlStorage },
				{ VeryShortMetaDescriptionUrlStorageType, &veryShortMetaDescriptionUrlStorage },
				{ SeveralMetaDescriptionUrlStorageType, &severalMetaDescriptionUrlStorage },

				{ EmptyMetaKeywordsUrlStorageType, &emptyMetaKeywordsUrlStorage },
				{ DuplicatedMetaKeywordsUrlStorageType, &duplicatedMetaKeywordsUrlStorage },
				{ SeveralMetaKeywordsUrlStorageType, &severalMetaKeywordsUrlStorage },

				{ MissingH1UrlStorageType, &missingH1UrlStorage },
				{ DuplicatedH1UrlStorageType, &duplicatedH1UrlStorage },
				{ VeryLongH1UrlStorageType, &veryLongH1UrlStorage },
				{ SeveralH1UrlStorageType, &severalH1UrlStorage },

				{ MissingH2UrlStorageType, &missingH2UrlStorage },
				{ DuplicatedH2UrlStorageType, &duplicatedH2UrlStorage },
				{ VeryLongH2UrlStorageType, &veryLongH2UrlStorage },
				{ SeveralH2UrlStorageType, &severalH2UrlStorage },

				{ Over100kbImageStorageType, &over100kbImageStorage },
				{ MissingAltTextImageStorageType, &missingAltTextImageStorage },
				{ VeryLongAltTextImageStorageType, &veryLongAltTextImageStorage }
			};
		}

		// internal crawler url queues
		StorageTypeUrl internalUrlStorage;
		StorageTypeUrl crawledUrlStorage;
		StorageTypeUrl externalUrlStorage;

		// urls
		StorageTypeUrl upperCaseUrlStorage;
		StorageTypeUrl nonAsciiCharacterUrlStorage;
		StorageTypeUrl veryLongUrlStorage;

		// titles
		StorageTypeTitle emptyTitleUrlStorage;
		StorageTypeTitle duplicatedTitleUrlStorage;
		StorageTypeTitle veryLongTitleUrlStorage;
		StorageTypeTitle veryShortTitleUrlStorage;
		StorageTypeTitle duplicatedH1TitleUrlStorage;
		StorageTypeTitle severalTitleUrlStorage;

		// meta descriptions
		StorageTypeMetaDescription emptyMetaDescriptionUrlStorage;
		StorageTypeMetaDescription duplicatedMetaDescriptionUrlStorage;
		StorageTypeMetaDescription veryLongMetaDescriptionUrlStorage;
		StorageTypeMetaDescription veryShortMetaDescriptionUrlStorage;
		StorageTypeMetaDescription severalMetaDescriptionUrlStorage;

		// meta keywords
		StorageTypeMetaKeywords emptyMetaKeywordsUrlStorage;
		StorageTypeMetaKeywords duplicatedMetaKeywordsUrlStorage;
		StorageTypeMetaKeywords severalMetaKeywordsUrlStorage;

		// H1
		StorageTypeH1 missingH1UrlStorage;
		StorageTypeH1 duplicatedH1UrlStorage;
		StorageTypeH1 veryLongH1UrlStorage;
		StorageTypeH1 severalH1UrlStorage;

		// H2
		StorageTypeH2 missingH2UrlStorage;
		StorageTypeH2 duplicatedH2UrlStorage;
		StorageTypeH2 veryLongH2UrlStorage;
		StorageTypeH2 severalH2UrlStorage;

		// images
		StorageTypeUrl over100kbImageStorage;
		StorageTypeUrl missingAltTextImageStorage;
		StorageTypeUrl veryLongAltTextImageStorage;

		std::map<int, void*> allStorages;
	};

private:
	Storage<CrawlerStorageTypeUrlHashed, CrawlerStorageTypeTitleHashed, CrawlerStorageTypeMetaDescriptionHashed, 
		CrawlerStorageTypeMetaKeywordsHashed, CrawlerStorageTypeH1Hashed, CrawlerStorageTypeH2Hashed> m_crawlerStorage;
	
	Storage<GuiStorageType, GuiStorageType, GuiStorageType, GuiStorageType, GuiStorageType, GuiStorageType> m_guiStorage;
};

}
