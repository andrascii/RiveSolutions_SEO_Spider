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
	, m_resizePolicy(std::make_unique<ViewportPercentResizePolicy>(std::vector<int>{ 85, 15 }))
	, m_hoveredIndex(QModelIndex())
	, m_selectionBgColor(97, 160, 50, 200)
	, m_hoveredBgColor("#a5a5a5")
	, m_bgColor(Qt::transparent)
{
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

	if (m_model->dataAccessor()->isHeaderItem(index))
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

void SummaryViewModel::resetRenderersCache() const noexcept
{
}

QList<IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	static SelectionBackgroundRenderer s_selectionBackgroundRenderer(this);
	static BackgroundRenderer s_backgroundRenderer(this);

	// disabled caching
	static TextRenderer s_textRenderer(this);

	return QList<IRenderer*>()
		<< &s_backgroundRenderer
		<< &s_selectionBackgroundRenderer
		<< &s_textRenderer;
}

void SummaryViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	m_hoveredIndex = index;
}

const QModelIndex& SummaryViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

QColor SummaryViewModel::textColor(const QModelIndex& index) const noexcept
{
	return QColor(Qt::black);
}

}