#include "page_info.h"
#include "itable_model.h"
#include "default_column_resize_policy.h"
#include "url_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "quickie_web_bot_helpers.h"
#include "table_view.h"


namespace QuickieWebBot
{

PageInfoStorageViewModel::PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_resizePolicy(std::make_unique<DefaultColumnResizePolicy>())
	, m_textRenderer(std::make_unique<TextRenderer>(this, static_cast<int>(std::pow((m_model->columnCount()), 2.0))))
	, m_urlRenderer(std::make_unique<UrlRenderer>(this, static_cast<int>(std::pow(m_model->columnCount(), 2.0))))
	, m_selectionBackgroundRenderer(std::make_unique<SelectionBackgroundRenderer>())
	, m_backgroundRenderer(std::make_unique<BackgroundRenderer>())
{
	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));

	std::map<int, int> sizes =
	{
		{ PageInfo::UrlItemType, PageInfo::columnPrefferedSize(PageInfo::UrlItemType) },
		{ PageInfo::ContentItemType, PageInfo::columnPrefferedSize(PageInfo::ContentItemType) },
		{ PageInfo::TitleItemType, PageInfo::columnPrefferedSize(PageInfo::TitleItemType) },
		{ PageInfo::TitleLengthItemType, PageInfo::columnPrefferedSize(PageInfo::TitleLengthItemType) },
		{ PageInfo::MetaRefreshItemType, PageInfo::columnPrefferedSize(PageInfo::MetaRefreshItemType) },
		{ PageInfo::MetaRobotsItemType, PageInfo::columnPrefferedSize(PageInfo::MetaRobotsItemType) },
		{ PageInfo::MetaDescriptionItemType, PageInfo::columnPrefferedSize(PageInfo::MetaDescriptionItemType) },
		{ PageInfo::MetaKeywordsItemType, PageInfo::columnPrefferedSize(PageInfo::MetaKeywordsItemType) },
		{ PageInfo::RedirectedUrlItemType, PageInfo::columnPrefferedSize(PageInfo::RedirectedUrlItemType) },
		{ PageInfo::FirstH1ItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH1ItemType) },
		{ PageInfo::SecondH1ItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH1ItemType) },
		{ PageInfo::FirstH2ItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH2ItemType) },
		{ PageInfo::SecondH2ItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH2ItemType) },
		{ PageInfo::CanonicalLinkElementItemType, PageInfo::columnPrefferedSize(PageInfo::CanonicalLinkElementItemType) },
		{ PageInfo::StatusCodeItemType, PageInfo::columnPrefferedSize(PageInfo::StatusCodeItemType) },
		{ PageInfo::PageSizeKbItemType, PageInfo::columnPrefferedSize(PageInfo::PageSizeKbItemType) },
		{ PageInfo::WordCountItemType, PageInfo::columnPrefferedSize(PageInfo::WordCountItemType) },
		{ PageInfo::PageHashItemType, PageInfo::columnPrefferedSize(PageInfo::PageHashItemType) },
		{ PageInfo::UrlLengthItemType, PageInfo::columnPrefferedSize(PageInfo::UrlLengthItemType) },
		{ PageInfo::MetaDescriptionLengthItemType, PageInfo::columnPrefferedSize(PageInfo::MetaDescriptionLengthItemType) },
		{ PageInfo::MetaKeywordsLengthItemType, PageInfo::columnPrefferedSize(PageInfo::MetaKeywordsLengthItemType) },
		{ PageInfo::FirstH1LengthItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH1LengthItemType) },
		{ PageInfo::SecondH1LengthItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH1LengthItemType) },
		{ PageInfo::FirstH2LengthItemType, PageInfo::columnPrefferedSize(PageInfo::FirstH2LengthItemType) },
		{ PageInfo::SecondH2LengthItemType, PageInfo::columnPrefferedSize(PageInfo::SecondH2LengthItemType) },
		{ PageInfo::ImageAltTextItemType, PageInfo::columnPrefferedSize(PageInfo::ImageAltTextItemType) },
		{ PageInfo::ImageAltTextLengthItemType, PageInfo::columnPrefferedSize(PageInfo::ImageAltTextLengthItemType) },
		{ PageInfo::ImageSizeKbItemType, PageInfo::columnPrefferedSize(PageInfo::ImageSizeKbItemType) }
	};

	m_resizePolicy->setColumnsSize(sizes);
}

int PageInfoStorageViewModel::marginTop() const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginBottom() const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginRight() const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginLeft() const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

void PageInfoStorageViewModel::resetRenderersCache() const noexcept
{
	m_textRenderer->resetCache();
	m_urlRenderer->resetCache();
	m_selectionBackgroundRenderer->resetCache();
	m_backgroundRenderer->resetCache();
}

QList<IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	IRenderer* renderer = 
		m_model->itemType(index) == PageInfo::UrlItemType ? 
		m_urlRenderer.get() : m_textRenderer.get();

	return QList<IRenderer*>()
		<< m_backgroundRenderer.get()
		<< m_selectionBackgroundRenderer.get()
		<< renderer;
}

void PageInfoStorageViewModel::onAttachedModelInternalDataChanged()
{
	m_textRenderer->setCacheSize(static_cast<int>(std::pow((m_model->columnCount()), 2.0)));
}

}