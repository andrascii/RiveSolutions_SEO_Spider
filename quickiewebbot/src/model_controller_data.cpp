#include "model_controller_data.h"

namespace QuickieWebBot
{

void ModelControllerData::checkStorageType(StorageType type) const noexcept
{
	assert(
		type == InternalUrlStorageType || 
		type == CrawledUrlStorageType || 
		type == ExternalUrlStorageType ||
		type == UpperCaseUrlStorageType ||
		type == NonAsciiCharacterUrlStorageType ||
		type == VeryLongUrlStorageType ||

		type == EmptyTitleUrlStorageType ||
		type == DuplicatedTitleUrlStorageType ||
		type == VeryLongTitleUrlStorageType ||
		type == VeryShortTitleUrlStorageType ||
		type == DuplicatedH1TitleUrlStorageType ||
		type == SeveralTitleUrlStorageType ||

		type == EmptyMetaDescriptionUrlStorageType ||
		type == DuplicatedMetaDescriptionUrlStorageType ||
		type == VeryLongMetaDescriptionUrlStorageType ||
		type == VeryShortMetaDescriptionUrlStorageType ||
		type == SeveralMetaDescriptionUrlStorageType ||

		type == EmptyMetaKeywordsUrlStorageType ||
		type == DuplicatedMetaKeywordsUrlStorageType ||
		type == SeveralMetaKeywordsUrlStorageType ||

		type == MissingH1UrlStorageType ||
		type == DuplicatedH1UrlStorageType ||
		type == VeryLongH1UrlStorageType ||
		type == SeveralH1UrlStorageType ||

		type == MissingH2UrlStorageType ||
		type == DuplicatedH2UrlStorageType ||
		type == VeryLongH2UrlStorageType ||
		type == SeveralH2UrlStorageType ||

		type == Over100kbImageStorageType ||
		type == MissingAltTextImageStorageType ||
		type == VeryLongAltTextImageStorageType
	);
}

ModelControllerData::GuiStorageType* ModelControllerData::guiStorage(StorageType type) noexcept
{
	GuiStorageType const* pQueue = const_cast<ModelControllerData const * const>(this)->guiStorage(type);
	return const_cast<GuiStorageType*>(pQueue);
}

ModelControllerData::GuiStorageType const* ModelControllerData::guiStorage(StorageType type) const noexcept
{
	GuiStorageType const* pQueue = nullptr;

	switch (type)
	{
		case InternalUrlStorageType:
		{
			pQueue = &m_guiStorage.internalUrlStorage;
			break;
		}

		case CrawledUrlStorageType:
		{
			pQueue = &m_guiStorage.crawledUrlStorage;
			break;
		}

		case ExternalUrlStorageType:
		{
			pQueue = &m_guiStorage.externalUrlStorage;
			break;
		}

		case UpperCaseUrlStorageType:
		{
			pQueue = &m_guiStorage.upperCaseUrlStorage;
			break;
		}

		case NonAsciiCharacterUrlStorageType:
		{
			pQueue = &m_guiStorage.nonAsciiCharacterUrlStorage;
			break;
		}

		case VeryLongUrlStorageType:
		{
			pQueue = &m_guiStorage.veryLongUrlStorage;
			break;
		}

		case EmptyTitleUrlStorageType:
		{
			pQueue = &m_guiStorage.emptyTitleUrlStorage;
			break;
		}

		case DuplicatedTitleUrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedTitleUrlStorage;
			break;
		}

		case VeryLongTitleUrlStorageType:
		{
			pQueue = &m_guiStorage.veryLongTitleUrlStorage;
			break;
		}

		case VeryShortTitleUrlStorageType:
		{
			pQueue = &m_guiStorage.veryShortTitleUrlStorage;
			break;
		}

		case DuplicatedH1TitleUrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedH1TitleUrlStorage;
			break;
		}

		case SeveralTitleUrlStorageType:
		{
			pQueue = &m_guiStorage.severalTitleUrlStorage;
			break;
		}

		case EmptyMetaDescriptionUrlStorageType:
		{
			pQueue = &m_guiStorage.emptyMetaDescriptionUrlStorage;
			break;
		}

		case DuplicatedMetaDescriptionUrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedMetaDescriptionUrlStorage;
			break;
		}

		case VeryLongMetaDescriptionUrlStorageType:
		{
			pQueue = &m_guiStorage.veryLongMetaDescriptionUrlStorage;
			break;
		}

		case VeryShortMetaDescriptionUrlStorageType:
		{
			pQueue = &m_guiStorage.veryShortMetaDescriptionUrlStorage;
			break;
		}

		case SeveralMetaDescriptionUrlStorageType:
		{
			pQueue = &m_guiStorage.severalMetaDescriptionUrlStorage;
			break;
		}

		case EmptyMetaKeywordsUrlStorageType:
		{
			pQueue = &m_guiStorage.emptyMetaKeywordsUrlStorage;
			break;
		}

		case DuplicatedMetaKeywordsUrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedMetaKeywordsUrlStorage;
			break;
		}

		case SeveralMetaKeywordsUrlStorageType:
		{
			pQueue = &m_guiStorage.severalMetaKeywordsUrlStorage;
			break;
		}

		case MissingH1UrlStorageType:
		{
			pQueue = &m_guiStorage.missingH1UrlStorage;
			break;
		}

		case DuplicatedH1UrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedH1UrlStorage;
			break;
		}

		case VeryLongH1UrlStorageType:
		{
			pQueue = &m_guiStorage.veryLongH1UrlStorage;
			break;
		}

		case SeveralH1UrlStorageType:
		{
			pQueue = &m_guiStorage.severalH1UrlStorage;
			break;
		}

		case MissingH2UrlStorageType:
		{
			pQueue = &m_guiStorage.missingH2UrlStorage;
			break;
		}

		case DuplicatedH2UrlStorageType:
		{
			pQueue = &m_guiStorage.duplicatedH2UrlStorage;
			break;
		}

		case VeryLongH2UrlStorageType:
		{
			pQueue = &m_guiStorage.veryLongH2UrlStorage;
			break;
		}

		case SeveralH2UrlStorageType:
		{
			pQueue = &m_guiStorage.severalH2UrlStorage;
			break;
		}

		case Over100kbImageStorageType:
		{
			pQueue = &m_guiStorage.over100kbImageStorage;
			break;
		}

		case MissingAltTextImageStorageType:
		{
			pQueue = &m_guiStorage.missingAltTextImageStorage;
			break;
		}

		case VeryLongAltTextImageStorageType:
		{
			pQueue = &m_guiStorage.veryLongAltTextImageStorage;
			break;
		}
	}

	return pQueue;
}

}