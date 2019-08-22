#include "stdafx.h"
#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "helpers.h"
#include "model_helpers.h"

namespace
{

#ifdef Q_OS_WIN
    const int s_headerSize = 12;
    const int s_filterTextSize = 10;
#else
    const int s_headerSize = 14;
    const int s_filterTextSize = 12;
#endif

}

namespace SeoSpider
{

SummaryViewModel::SummaryViewModel(QWidget* parentView, SummaryModel* model, float devicePixelRatio, QObject* parent)
	: AbstractViewModel(model, devicePixelRatio, parent)
	, m_selectedBackgroundColor("#E5E5E5")
	, m_hoveredBackgroundColor("#F3F3F3")
	, m_backgroundColor("#FFFFFF")
	, m_selectedGridLineColor("#E5E5E5")
	, m_gridLineColor("#F3F3F3")
	, m_headerTextColor("#2C2D30")
	, m_textColor("#2C2D30")
	, m_headerFont("Arial", s_headerSize, QFont::DemiBold)
	, m_textFont("Arial", s_filterTextSize, QFont::Medium)
	, m_itemRenderer(this)
	, m_parentView(parentView)
	, m_windowBlocked(false)
{
	initializeRenderers();

	VERIFY(connect(model, SIGNAL(internalDataChanged()), this, SLOT(onDataWereReset())));
	VERIFY(connect(model, &SummaryModel::dataChanged, this, &SummaryViewModel::onAttachedModelDataChanged));
	parentView->installEventFilter(this);
}

int SummaryViewModel::marginTop(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(4) * devicePixelRatio();
}

int SummaryViewModel::marginBottom(const QModelIndex& ) const noexcept
{
	return Common::Helpers::pointsToPixels(0);
}

int SummaryViewModel::marginLeft(const QModelIndex& index) const noexcept
{
    if (index.column() == 1)
    {
        return Common::Helpers::pointsToPixels(0);
    }

	return Common::Helpers::pointsToPixels(3) * devicePixelRatio();
}

int SummaryViewModel::marginRight(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(3);
}

QPixmap SummaryViewModel::pixmap(const QModelIndex& index) const noexcept
{
	const SummaryModel* model = static_cast<const SummaryModel*>(AbstractViewModel::model());

	const QPixmap& originalPixmap = model->dataAccessor()->pixmap(index);

	if (m_windowBlocked)
	{
		QIcon icon(originalPixmap);

		return icon.pixmap(originalPixmap.size(), QIcon::Disabled, QIcon::Off);
	}

	return originalPixmap;
}

QRect SummaryViewModel::pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect.adjusted(marginLeft(index), marginTop(index), -marginRight(index), -marginBottom(index));
}

QString SummaryViewModel::displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	// DEBUG_ASSERT(index.model() == static_cast<const QAbstractItemModel*>(m_model));

	const QString displayData = index.data(Qt::DisplayRole).toString();
	QFontMetrics fontMetrics(font(index));
	return fontMetrics.elidedText(displayData, Qt::ElideRight, itemVisualRect.width());
}

QRect SummaryViewModel::displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
    if (index.column() == 1)
    {
        return itemVisualRect.adjusted(
            marginLeft(index),
            marginTop(index),
            -marginRight(index) - itemVisualRect.width() / devicePixelRatio(),
            -marginBottom(index));
    }

	return itemVisualRect.adjusted(
	    pixmap(index).width() + Common::Helpers::pointsToPixels(3) + marginLeft(index),
	    marginTop(index),
	    -marginRight(index),
	    -marginBottom(index));
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
		const QModelIndexList& modelIndexes = makeRowIndexes(previousHoveredIndex);

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}

	if (hoveredIndex().isValid())
	{
		const QModelIndexList& modelIndexes = makeRowIndexes(hoveredIndex());

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}


bool SummaryViewModel::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_parentView &&
		(event->type() == QEvent::WindowActivate ||
		event->type() == QEvent::WindowDeactivate))
	{
		const bool isWindowBlocked = QApplication::activeModalWidget() != nullptr;

		if (isWindowBlocked != m_windowBlocked)
		{
			m_windowBlocked = isWindowBlocked;
			invalidateItemViewRendererCache();
		}
	}

	return AbstractViewModel::eventFilter(object, event);
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
