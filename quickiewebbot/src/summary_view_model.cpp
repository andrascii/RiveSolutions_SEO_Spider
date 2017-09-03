#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "quickie_web_bot_helpers.h"
#include "viewport_percent_resize_policy.h"

namespace QuickieWebBot
{

SummaryViewModel::SummaryViewModel(SummaryModel* model, QObject* parent)
	: m_model(model)
	, m_hoveredIndex(QModelIndex())
	, m_selectionBgColor("#C0C0C0")
	, m_hoveredBgColor("#F3F3F3")
	, m_bgColor(Qt::transparent)
{
	initializeRenderers();
}

int SummaryViewModel::marginTop(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int SummaryViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int SummaryViewModel::marginLeft(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int SummaryViewModel::marginRight(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

const QPixmap& SummaryViewModel::itemPixmap(const QModelIndex& index) const noexcept
{
	return m_model->dataAccessor()->pixmap(index);
}

QRect SummaryViewModel::itemPixmapPosition(const QModelIndex&) const noexcept
{
	return QRect();
}

const QColor& SummaryViewModel::selectionBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBgColor;
}

const QColor& SummaryViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBgColor;
}

const QColor& SummaryViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_bgColor;
}

const QFont& SummaryViewModel::font(const QModelIndex& index) const noexcept
{
	static QFont font;

	font.setBold(false);

	if (m_model->dataAccessor()->isHeaderRow(index.row()))
	{
		font.setBold(true);
	}

	return font;
}

Qt::AlignmentFlag SummaryViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() != 0)
	{
		return Qt::AlignRight;
	}

	return Qt::AlignLeft;
}

void SummaryViewModel::invalidateRenderersCache() const noexcept
{
}

QList<const IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	return QList<const IRenderer*>()
		<< m_renderers.find(IViewModel::BackgroundRendererType)->second.get()
		<< m_renderers.find(IViewModel::SelectionBackgroundRendererType)->second.get()
		<< m_renderers.find(IViewModel::PlainTextRendererType)->second.get();
}

void SummaryViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	if (index == m_hoveredIndex)
	{
		return;
	}
	
	const QModelIndex previousHoveredIndex = m_hoveredIndex;

	m_hoveredIndex = index;

	if (previousHoveredIndex.isValid())
	{
		const QModelIndexList& modelIndexes = m_model->modelIndexesForRow(previousHoveredIndex.row());

		invalidateCacheIndexes(modelIndexes);

		Q_EMIT repaintItems(modelIndexes);
	}

	if (m_hoveredIndex.isValid())
	{
		const QModelIndexList& modelIndexes = m_model->modelIndexesForRow(m_hoveredIndex.row());

		invalidateCacheIndexes(m_model->modelIndexesForRow(m_hoveredIndex.row()));

		Q_EMIT repaintItems(modelIndexes);
	}

	DEBUGLOG << "current hovered row" << m_hoveredIndex.row();
}

const QModelIndex& SummaryViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

QObject* SummaryViewModel::qobject() noexcept
{
	return this;
}

void SummaryViewModel::invalidateCacheIndexes(const QModelIndexList& indexesList)
{
	foreach(const QModelIndex& index, indexesList)
	{
		invalidateCacheIndex(index);
	}
}

void SummaryViewModel::invalidateCacheIndex(const QModelIndex& index)
{
	for (const auto& pair : m_renderers)
	{
		pair.second->invalidateCacheIndex(index);
	}
}

void SummaryViewModel::initializeRenderers()
{
	m_renderers[IViewModel::PlainTextRendererType] =
		std::make_unique<TextRenderer>(this);

	m_renderers[IViewModel::SelectionBackgroundRendererType] =
		std::make_unique<SelectionBackgroundRenderer>(this);

	m_renderers[IViewModel::BackgroundRendererType] =
		std::make_unique<BackgroundRenderer>(this, m_model->rowCount() * m_model->columnCount());
}

QColor SummaryViewModel::textColor(const QModelIndex& index) const noexcept
{
	return QColor(Qt::black);
}

}