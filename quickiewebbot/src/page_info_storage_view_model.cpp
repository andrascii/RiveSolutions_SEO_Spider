#include "page_info.h"
#include "abstract_table_model.h"
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
	, m_hoveredIndex(QModelIndex())
	, m_selectionBgColor(97, 160, 50, 200)
	, m_hoveredBgColor("#a5a5a5")
	, m_bgColor(Qt::white)
{
	initializeRenderers();

	QPixmap pixmap;
	pixmap.fill(backgroundColor(QModelIndex()));
	pixmap.load(":/images/url-icon.png");

	m_urlIcon = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(20), QuickieWebBotHelpers::pointsToPixels(20));

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
	if (m_model->itemType(index) == PageInfo::UrlItemType)
	{
		return QColor("#6F99C8");
	}

	return QColor("#02072E");
}

void PageInfoStorageViewModel::invalidateRenderersCache() const noexcept
{
	for (const auto& pair : m_renderers)
	{
		pair.second->invalidateCache();
	}
}

QList<const IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	const IRenderer* renderer = m_model->itemType(index) == PageInfo::UrlItemType ?
		m_renderers.find(IViewModel::UrlRendererType)->second.get() :
		m_renderers.find(IViewModel::PlainTextRendererType)->second.get();

	return QList<const IRenderer*>()
		<< m_renderers.find(IViewModel::BackgroundRendererType)->second.get()
		<< m_renderers.find(IViewModel::SelectionBackgroundRendererType)->second.get()
		<< renderer;
}

void PageInfoStorageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	if (index == m_hoveredIndex)
	{
		return;
	}

	m_hoveredIndex = index;
}

const QModelIndex& PageInfoStorageViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

QObject* PageInfoStorageViewModel::qobject() noexcept
{
	return this;
}

void PageInfoStorageViewModel::onAttachedModelInternalDataChanged()
{
	m_renderers[IViewModel::PlainTextRendererType]->setCacheSize(static_cast<int>(std::pow((m_model->columnCount()), 2.0)));
}

void PageInfoStorageViewModel::invalidateCacheIndex(const QModelIndex& index)
{
	for (const auto& pair : m_renderers)
	{
		pair.second->invalidateCacheIndex(index);
	}
}

void PageInfoStorageViewModel::initializeRenderers()
{
	m_renderers[IViewModel::PlainTextRendererType] =
		std::make_unique<TextRenderer>(this, static_cast<int>(std::pow((m_model->columnCount()), 2.0)));

	m_renderers[IViewModel::UrlRendererType] =
		std::make_unique<UrlRenderer>(this, static_cast<int>(std::pow(m_model->columnCount(), 2.0)));

	m_renderers[IViewModel::SelectionBackgroundRendererType] =
		std::make_unique<SelectionBackgroundRenderer>(this);

	m_renderers[IViewModel::BackgroundRendererType] = 
		std::make_unique<BackgroundRenderer>(this);
}

}