#include "page_info.h"
#include "abstract_table_model.h"
#include "default_column_resize_policy.h"
#include "url_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "grid_line_renderer.h"
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
	, m_selectionBgColor("#C0C0C0")
	, m_hoveredBgColor("#F3F3F3")
	, m_bgColor(Qt::white)
{
	initializeRenderers();

	QPixmap pixmap;
	pixmap.fill(backgroundColor(QModelIndex()));
	pixmap.load(":/images/url-icon.png");

	m_urlIcon = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(20), QuickieWebBotHelpers::pointsToPixels(20));

	VERIFY(connect(m_model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));
}

int PageInfoStorageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(4);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int PageInfoStorageViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(2);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
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
	static QFont font = Application::font();

	return font;
}

Qt::AlignmentFlag PageInfoStorageViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Qt::AlignCenter;
	}

	return Qt::AlignLeft;
}

QColor PageInfoStorageViewModel::textColor(const QModelIndex& index) const noexcept
{
	if (m_model->itemType(index) == PageInfo::UrlItemType)
	{
		return QColor("#1754A8");
	}

	return QColor("#000000");
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
		m_renderers.find(UrlRendererType)->second.get() :
		m_renderers.find(PlainTextRendererType)->second.get();

	return QList<const IRenderer*>()
		<< m_renderers.find(BackgroundRendererType)->second.get()
		<< m_renderers.find(SelectionBackgroundRendererType)->second.get()
		<< renderer
		<< m_renderers.find(GridLineRendererType)->second.get();
}

void PageInfoStorageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	if (index == m_hoveredIndex)
	{
		return;
	}

	const QModelIndex previousHoveredIndex = m_hoveredIndex;

	m_hoveredIndex = index;

	if (previousHoveredIndex.isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(m_model->modelIndexesForRow(previousHoveredIndex.row()));
		modelIndexes.append(m_model->modelIndexesForRow(previousHoveredIndex.row() - 1));

		invalidateCacheIndexes(modelIndexes);

		Q_EMIT repaintItems(modelIndexes);
	}

	if (m_hoveredIndex.isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(m_model->modelIndexesForRow(m_hoveredIndex.row()));
		modelIndexes.append(m_model->modelIndexesForRow(m_hoveredIndex.row() - 1));

		invalidateCacheIndexes(modelIndexes);

		Q_EMIT repaintItems(modelIndexes);
	}
}

const QModelIndex& PageInfoStorageViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

void PageInfoStorageViewModel::setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (!m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			m_selectedModelIndexes.append(modelIndexes[i]);
		}
	}

	invalidateCacheIndexes(m_selectedModelIndexes);

	Q_EMIT repaintItems(m_selectedModelIndexes);
}

void PageInfoStorageViewModel::setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			m_selectedModelIndexes.removeOne(modelIndexes[i]);
		}
	}

	invalidateCacheIndexes(m_selectedModelIndexes);

	Q_EMIT repaintItems(m_selectedModelIndexes);
}

const QModelIndexList& PageInfoStorageViewModel::selectedIndexes() const noexcept
{
	return m_selectedModelIndexes;
}

QObject* PageInfoStorageViewModel::qobject() noexcept
{
	return this;
}

void PageInfoStorageViewModel::onAttachedModelInternalDataChanged()
{
	m_renderers[PlainTextRendererType]->setCacheSize(static_cast<int>(std::pow((m_model->columnCount()), 2.0)));
}

void PageInfoStorageViewModel::invalidateCacheIndexes(const QModelIndexList& indexesList)
{
	foreach(const QModelIndex& index, indexesList)
	{
		invalidateCacheIndex(index);
	}
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
	m_renderers[PlainTextRendererType] = std::make_unique<TextRenderer>(this, static_cast<int>(std::pow((m_model->columnCount()), 2.0)));
	m_renderers[UrlRendererType] = std::make_unique<UrlRenderer>(this, static_cast<int>(std::pow(m_model->columnCount(), 2.0)));
	m_renderers[SelectionBackgroundRendererType] = std::make_unique<SelectionBackgroundRenderer>(this);
	m_renderers[BackgroundRendererType] = std::make_unique<BackgroundRenderer>(this);
	m_renderers[GridLineRendererType] = std::make_unique<GridLineRenderer>(this, QColor("#F1F1F1"));
}

}