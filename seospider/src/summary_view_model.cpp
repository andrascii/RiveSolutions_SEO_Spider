#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "helpers.h"

namespace SeoSpider
{

SummaryViewModel::SummaryViewModel(SummaryModel* model, QObject* parent)
	: AbstractViewModel(model, parent)
	, m_selectedBackgroundColor(/*"#F7F0D6"*/"#D2D3D5")
	, m_hoveredBackgroundColor("#F2F2F2")
	, m_backgroundColor("#FCFDFE")
	, m_selectedGridLineColor("#F7F0D6")
	, m_gridLineColor("#F3F3F3")
	, m_headerTextColor("#000000")
	, m_textColor("#000000")
	, m_headerFont("Helvetica", 10, QFont::DemiBold)
	, m_textFont("Helvetica", 10, QFont::Normal)
	, m_itemRenderer(this)
{
	initializeRenderers();

	connect(model, &SummaryModel::internalDataChanged, this, &SummaryViewModel::onDataWereReset);
}

int SummaryViewModel::marginTop(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(5);
}

int SummaryViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(0);
}

int SummaryViewModel::marginLeft(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(6);
}

int SummaryViewModel::marginRight(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(2);
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
	QFontMetrics fontMetrics(font(index));
	return fontMetrics.elidedText(displayData, Qt::ElideRight, itemVisualRect.width());
}

QRect SummaryViewModel::displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect.adjusted(pixmap(index).width() + Common::Helpers::pointsToPixels(3), 0, 0, 0);
}

const QColor& SummaryViewModel::selectedBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectedBackgroundColor;
}

const QColor& SummaryViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBackgroundColor;
}

const QColor& SummaryViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_backgroundColor;
}

const QFont& SummaryViewModel::font(const QModelIndex& index) const noexcept
{
	const SummaryModel* model = static_cast<const SummaryModel*>(AbstractViewModel::model());

	if (model->dataAccessor()->isHeaderRow(index.row()))
	{
		return m_headerFont;
	}

	return m_textFont;
}

int SummaryViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() != 0)
	{
		return Qt::AlignRight;
	}

	return Qt::AlignLeft;
}

void SummaryViewModel::onDataWereReset()
{
	AbstractViewModel::invalidateItemViewRendererCache();
}

void SummaryViewModel::initializeRenderers()
{
	AbstractViewModel::addRenderer(
		IRenderer::PlainTextRendererType | 
		IRenderer::SelectionBackgroundRendererType | 
		IRenderer::BackgroundRendererType
	);
}

const QColor& SummaryViewModel::textColor(const QModelIndex& index) const noexcept
{
	const SummaryModel* model = static_cast<const SummaryModel*>(AbstractViewModel::model());

	if (model->dataAccessor()->isHeaderRow(index.row()))
	{
		return m_headerTextColor;
	}

	return m_textColor;
}

const QColor& SummaryViewModel::selectedGridLineColor(const QModelIndex&) const noexcept
{
	return m_selectedBackgroundColor;
}

const QColor& SummaryViewModel::gridLineColor(const QModelIndex&) const noexcept
{
	return m_gridLineColor;
}

void SummaryViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	AbstractViewModel::setHoveredIndex(index);

	QModelIndex previousHoveredIndex = AbstractViewModel::previousHoveredIndex();

	if (previousHoveredIndex.isValid())
	{
		const QModelIndexList& modelIndexes = model()->makeModelIndexesForRow(previousHoveredIndex.row());

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}

	if (hoveredIndex().isValid())
	{
		const QModelIndexList& modelIndexes = model()->makeModelIndexesForRow(hoveredIndex().row());

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}


const QColor& SummaryViewModel::backgroundColor() const noexcept
{
	return m_backgroundColor;
}

void SummaryViewModel::setBackgroundColor(const QColor& color) noexcept
{
	m_backgroundColor = color;

	emit backgroundColorChanged();
}

const QColor& SummaryViewModel::textColor() const noexcept
{
	return m_textColor;
}

void SummaryViewModel::setTextColor(const QColor& color) noexcept
{
	m_textColor = color;

	emit textColorChanged();
}

const QColor& SummaryViewModel::selectedBackgroundColor() const noexcept
{
	return m_selectedBackgroundColor;
}

void SummaryViewModel::setSelectedBackgroundColor(const QColor& color) noexcept
{
	m_selectedBackgroundColor = color;

	emit selectedBackgroundColorChanged();
}

const QColor& SummaryViewModel::hoveredBackgroundColor() const noexcept
{
	return m_hoveredBackgroundColor;
}

void SummaryViewModel::setHoveredBackgroundColor(const QColor& color) noexcept
{
	m_hoveredBackgroundColor = color;

	emit hoveredBackgroundColorChanged();
}

const QColor& SummaryViewModel::selectedGridLineColor() const noexcept
{
	return m_selectedGridLineColor;
}

void SummaryViewModel::setSelectedGridLineColor(const QColor& color) noexcept
{
	m_selectedGridLineColor = color;

	emit selectedGridLineColorChanged();
}

const QColor& SummaryViewModel::gridLineColor() const noexcept
{
	return m_gridLineColor;
}

void SummaryViewModel::setGridLineColor(const QColor& color) noexcept
{
	m_gridLineColor = color;

	emit gridLineColorChanged();
}

const QFont& SummaryViewModel::headerFont() const noexcept
{
	return m_headerFont;
}

const QColor& SummaryViewModel::headerTextColor() const noexcept
{
	return m_headerTextColor;
}

void SummaryViewModel::setHeaderTextColor(const QColor& color) noexcept
{
	m_headerTextColor = color;

	emit headerTextColorChanged();
}

void SummaryViewModel::setHeaderFont(const QFont& font) noexcept
{
	m_headerFont = font;

	emit headerFontChanged();
}

const QFont& SummaryViewModel::textFont() const noexcept
{
	return m_textFont;
}

void SummaryViewModel::setTextFont(const QFont& font) noexcept
{
	m_textFont = font;

	emit textFontChanged();
}

}