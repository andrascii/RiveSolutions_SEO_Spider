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
	: AbstractViewModel(model, parent)
	, m_selectionBgColor("#E6EE9C")
	, m_hoveredBgColor("#F3F3F3")
	, m_bgColor(Qt::transparent)
	, m_itemRenderer(this)
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

const QPixmap& SummaryViewModel::pixmap(const QModelIndex& index) const noexcept
{
	const SummaryModel* model = static_cast<const SummaryModel*>(AbstractViewModel::model());

	return model->dataAccessor()->pixmap(index);
}

QRect SummaryViewModel::pixmapPosition(const QModelIndex&, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect;
}

QString SummaryViewModel::displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	DEBUG_ASSERT(index.model() == static_cast<const QAbstractItemModel*>(model()));

	const QString displayData = index.data(Qt::DisplayRole).toString();
	QFontMetrics fontMetrics(Application::font());
	return fontMetrics.elidedText(displayData, Qt::ElideRight, itemVisualRect.width());
}

QRect SummaryViewModel::displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect.adjusted(pixmap(index).width() + QuickieWebBotHelpers::pointsToPixels(3), 0, 0, 0);
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
	const SummaryModel* model = static_cast<const SummaryModel*>(AbstractViewModel::model());

	static QFont font;

	font.setBold(false);

	if (model->dataAccessor()->isHeaderRow(index.row()))
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

void SummaryViewModel::initializeRenderers()
{
	AbstractViewModel::addRenderer(
		IRenderer::PlainTextRendererType | 
		IRenderer::SelectionBackgroundRendererType | 
		IRenderer::BackgroundRendererType
	);

	AbstractViewModel::setItemRendererCacheSize(static_cast<int>(model()->columnCount() * model()->rowCount() * 2));
}

QColor SummaryViewModel::textColor(const QModelIndex& index) const noexcept
{
	return QColor(Qt::black);
}

void SummaryViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	AbstractViewModel::setHoveredIndex(index);

	QModelIndex previousHoveredIndex = AbstractViewModel::previousHoveredIndex();

	if (previousHoveredIndex.isValid())
	{
		const QModelIndexList& modelIndexes = model()->modelIndexesForRow(previousHoveredIndex.row());

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}

	if (hoveredIndex().isValid())
	{
		const QModelIndexList& modelIndexes = model()->modelIndexesForRow(hoveredIndex().row());

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}

}