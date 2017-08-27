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
	, m_selectionBackgroundRenderer(std::make_unique<SelectionBackgroundRenderer>(this))
	, m_backgroundRenderer(std::make_unique<BackgroundRenderer>(this))
	, m_hoveredIndex(QModelIndex())
	, m_selectionBgColor(97, 160, 50, 200)
	, m_hoveredBgColor("#a5a5a5")
	, m_bgColor(Qt::white)
{
	QPixmap pixmap{ ":/images/url-icon.png" };

	m_urlIcon = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(13.5), QuickieWebBotHelpers::pointsToPixels(13.5));

	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));
}

int PageInfoStorageViewModel::marginTop(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginLeft(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginRight(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

const QPixmap& PageInfoStorageViewModel::itemPixmap(const QModelIndex& index) const noexcept
{
	static QPixmap emptyPixmap;

	if (m_model->itemType(index) == PageInfo::UrlItemType)
	{
		return m_urlIcon;
	}

	return emptyPixmap;
}

QRect PageInfoStorageViewModel::itemPixmapPosition(const QModelIndex&) const noexcept
{
	return QRect();
}

const QColor& PageInfoStorageViewModel::selectionBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBgColor;
}

const QColor& PageInfoStorageViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBgColor;
}

const QColor& PageInfoStorageViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_bgColor;
}

const QFont& PageInfoStorageViewModel::font(const QModelIndex& index) const noexcept
{
	static QFont font;

	font.setBold(false);

	if (m_model->itemType(index) == PageInfo::UrlItemType)
	{
		font.setBold(true);
	}

	return font;
}

Qt::AlignmentFlag PageInfoStorageViewModel::textAlignment(const QModelIndex&) const noexcept
{
	return Qt::AlignLeft;
}

QColor PageInfoStorageViewModel::textColor(const QModelIndex& index) const noexcept
{
	return QColor(Qt::black);
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

void PageInfoStorageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	m_hoveredIndex = index;
}

const QModelIndex& PageInfoStorageViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

void PageInfoStorageViewModel::onAttachedModelInternalDataChanged()
{
	m_textRenderer->setCacheSize(static_cast<int>(std::pow((m_model->columnCount()), 2.0)));
}

}