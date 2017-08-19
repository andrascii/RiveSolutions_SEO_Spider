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
	, m_textRenderer(std::make_unique<TextRenderer>(this, static_cast<int>(std::pow((m_model->columnCount()), 2.0))))
	, m_urlRenderer(std::make_unique<UrlRenderer>(this, static_cast<int>(std::pow(m_model->columnCount(), 2.0))))
	, m_selectionBackgroundRenderer(std::make_unique<SelectionBackgroundRenderer>())
	, m_backgroundRenderer(std::make_unique<BackgroundRenderer>())
{
	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));
}

int PageInfoStorageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginBottom(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginRight(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginLeft(const QModelIndex& index) const noexcept
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