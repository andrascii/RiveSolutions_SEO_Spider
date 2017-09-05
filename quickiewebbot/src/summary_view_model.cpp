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
	const SummaryModel* model = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(AbstractViewModel::model());

	return model->dataAccessor()->pixmap(index);
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
	const SummaryModel* model = QuickieWebBotHelpers::safe_runtime_static_cast<const SummaryModel*>(AbstractViewModel::model());

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

QList<const IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	return QList<const IRenderer*>()
		<< AbstractViewModel::renderer(AbstractViewModel::BackgroundRendererType)
		<< AbstractViewModel::renderer(AbstractViewModel::SelectionBackgroundRendererType)
		<< AbstractViewModel::renderer(AbstractViewModel::PlainTextRendererType);
}

void SummaryViewModel::initializeRenderers()
{
	AbstractViewModel::addRenderer(AbstractViewModel::PlainTextRendererType, new TextRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::SelectionBackgroundRendererType, new SelectionBackgroundRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::BackgroundRendererType, new BackgroundRenderer(this, model()->rowCount() * model()->columnCount()));
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