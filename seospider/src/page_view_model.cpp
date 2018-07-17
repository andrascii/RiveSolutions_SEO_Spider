#include "page_model.h"
#include "page_view_model.h"
#include "helpers.h"
#include "url.h"
#include "svg_renderer.h"
#include "model_helpers.h"

namespace
{

constexpr int s_preferredCacheSize = 500;

}

namespace SeoSpider
{

PageViewModel::PageViewModel(QWidget* parentView, PageModel* model, QObject* parent)
	: AbstractViewModel(model, parent)
	, m_parentView(parentView)
	, m_selectionBackgroundColor("#E5E5E5")
	, m_hoveredBackgroundColor("#F3F3F3")
	, m_backgroundColor("#FFFFFF")
	, m_selectedGridLineColor("#E5E5E5")
	, m_gridLineColor("#F3F3F3")
	, m_urlTextColor("#1C51AF")
	, m_textColor("#2C2D30")
	, m_textFont("Lato", 10, QFont::Normal)
	, m_itemRenderer(this)
{
	initializeRenderers();

	m_urlIcon = SvgRenderer::render(QStringLiteral(":/images/go-to-url.svg"), 13, 13);

	VERIFY(connect(model, &PageModel::internalDataChanged, this, &PageViewModel::onAttachedModelStorageAdapterChanged));
	VERIFY(connect(model, &PageModel::dataChanged, this, &PageViewModel::onAttachedModelDataChanged));
	VERIFY(connect(model, &PageModel::modelReset, this, &PageViewModel::onModelDataWasReset));
	VERIFY(connect(model, &PageModel::rowsAboutToBeRemoved, this, &PageViewModel::onRowsAboutToBeRemoved));
	VERIFY(connect(model, &PageModel::rowsAboutToBeMoved, this, &PageViewModel::onRowsAboutToBeMoved));
}

int PageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Common::Helpers::pointsToPixels(0);
	}

	return Common::Helpers::pointsToPixels(0);
}

int PageViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(0);
}

int PageViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Common::Helpers::pointsToPixels(2);
	}

	return Common::Helpers::pointsToPixels(6);
}

int PageViewModel::marginRight(const QModelIndex&) const noexcept
{
	return Common::Helpers::pointsToPixels(2);
}

QPixmap PageViewModel::pixmap(const QModelIndex& index) const noexcept
{
	const PageModel* model = 
		static_cast<const PageModel*>(AbstractViewModel::model());

	static QPixmap s_emptyPixmap;

	QPixmap pixmap;

	if (model->itemType(index) == IStorageAdapter::ItemType::UrlItemType && 
		hoveredIndex().row() == index.row() &&
		CrawlerEngine::Url(index.data(Qt::DisplayRole).toString()).isValid())
	{
		pixmap = m_urlIcon;
	}
	else
	{
		pixmap = s_emptyPixmap;
	}

	if (!m_parentView->isEnabled())
	{
		QIcon icon(pixmap);

		return icon.pixmap(pixmap.size(), QIcon::Disabled, QIcon::Off);
	}

	return pixmap;
}

QRect PageViewModel::pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	const PageModel* model =
		static_cast<const PageModel*>(AbstractViewModel::model());

	const QPixmap& pix = pixmap(index);
	const int visualRectHeight = itemVisualRect.height();

	const int offsetByY = (visualRectHeight - pix.height()) / 2;

	if (model->itemType(index) == IStorageAdapter::ItemType::UrlItemType)
	{
		return itemVisualRect.adjusted(itemVisualRect.width() - Common::Helpers::pointsToPixels(20), offsetByY, 0, 0);
	}

	return itemVisualRect.adjusted(itemVisualRect.width() + Common::Helpers::pointsToPixels(3), offsetByY, 0, 0);
}

QString PageViewModel::displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	// DEBUG_ASSERT(index.model() == static_cast<const QAbstractItemModel*>(model()));

	const QString displayData = index.data(Qt::DisplayRole).toString();

	const int pixmapOccupiedWidth = index.row() == hoveredIndex().row() ? itemVisualRect.width() - pixmapPosition(index, itemVisualRect).x() : 0;

	QFontMetrics fontMetrics(font(index));
	return fontMetrics.elidedText(displayData, Qt::ElideRight, itemVisualRect.width() - pixmapOccupiedWidth);
}

QRect PageViewModel::displayDataPosition(const QModelIndex&, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect;
}

const QColor& PageViewModel::selectedBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBackgroundColor;
}

const QColor& PageViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBackgroundColor;
}

const QColor& PageViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_backgroundColor;
}

const QFont& PageViewModel::font(const QModelIndex& index) const noexcept
{
	Q_UNUSED(index);

	return m_textFont;
}

int PageViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Qt::AlignCenter;
	}

	return Qt::AlignVCenter | Qt::AlignLeft;
}

const QColor& PageViewModel::textColor(const QModelIndex& index) const noexcept
{
	const PageModel* model = 
		static_cast<const PageModel*>(AbstractViewModel::model());

	if (model->itemType(index) == IStorageAdapter::ItemType::UrlItemType)
	{
		return m_urlTextColor;
	}

	return m_textColor;
}

const QColor& PageViewModel::selectedGridLineColor(const QModelIndex&) const noexcept
{
	return m_selectedGridLineColor;
}

const QColor& PageViewModel::gridLineColor(const QModelIndex&) const noexcept
{
	return m_gridLineColor;
}

void PageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	AbstractViewModel::setHoveredIndex(index);

	QModelIndex previousHoveredIndex = AbstractViewModel::previousHoveredIndex();

	if (previousHoveredIndex.isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(makeRowIndexes(previousHoveredIndex));
		modelIndexes.append(makeRowIndexes(previousHoveredIndex.model()->index(previousHoveredIndex.row() - 1, previousHoveredIndex.column())));

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
		AbstractViewModel::resetPreviousHoveredIndex();
	}

	if (hoveredIndex().isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(makeRowIndexes(hoveredIndex()));
		modelIndexes.append(makeRowIndexes(hoveredIndex().model()->index(hoveredIndex().row() - 1, hoveredIndex().column())));

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}

const QColor& PageViewModel::backgroundColor() const noexcept
{
	return m_backgroundColor;
}

void PageViewModel::setBackgroundColor(const QColor& color) noexcept
{
	m_backgroundColor = color;

	emit backgroundColorChanged();
}

const QColor& PageViewModel::textColor() const noexcept
{
	return m_textColor;
}

void PageViewModel::setTextColor(const QColor& color) noexcept
{
	m_textColor = color;

	emit textColorChanged();
}

const QColor& PageViewModel::selectionBackgroundColor() const noexcept
{
	return m_selectionBackgroundColor;
}

void PageViewModel::setSelectionBackgroundColor(const QColor& color) noexcept
{
	m_selectionBackgroundColor = color;

	emit selectionBackgroundColorChanged();
}

const QColor& PageViewModel::hoveredBackgroundColor() const noexcept
{
	return m_hoveredBackgroundColor;
}

void PageViewModel::setHoveredBackgroundColor(const QColor& color) noexcept
{
	m_hoveredBackgroundColor = color;

	emit hoveredBackgroundColorChanged();
}

const QColor& PageViewModel::selectedGridLineColor() const noexcept
{
	return m_selectedGridLineColor;
}

void PageViewModel::setSelectedGridLineColor(const QColor& color) noexcept
{
	m_selectedGridLineColor = color;

	emit selectedGridLineColorChanged();
}

const QColor& PageViewModel::gridLineColor() const noexcept
{
	return m_gridLineColor;
}

void PageViewModel::setGridLineColor(const QColor& color) noexcept
{
	m_gridLineColor = color;

	emit gridLineColorChanged();
}

const QColor& PageViewModel::urlTextColor() const noexcept
{
	return m_urlTextColor;
}

void PageViewModel::setUrlTextColor(const QColor& color) noexcept
{
	m_urlTextColor = color;

	emit urlTextColorChanged();
}

const QFont& PageViewModel::textFont() const noexcept
{
	return m_textFont;
}

void PageViewModel::setTextFont(const QFont& font) noexcept
{
	m_textFont = font;

	emit textFontChanged();
}

void PageViewModel::onAttachedModelStorageAdapterChanged()
{
	const PageModel* model =
		static_cast<const PageModel*>(AbstractViewModel::model());

	AbstractViewModel::clearSelectedIndexes();
	AbstractViewModel::invalidateItemViewRendererCache();
	const int cacheSize = static_cast<int>(model->columnCount() * model->columnCount());
	AbstractViewModel::setItemRendererCacheSize(std::max(cacheSize, 1));
}

void PageViewModel::onModelDataWasReset()
{
	invalidateItemViewRendererCache();
}

void PageViewModel::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);
	invalidateItemViewRendererCache();
}

void PageViewModel::onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
	const QModelIndex& destinationParent, int destinationRow)
{
	Q_UNUSED(sourceParent);
	Q_UNUSED(sourceStart);
	Q_UNUSED(sourceEnd);
	Q_UNUSED(destinationParent);
	Q_UNUSED(destinationRow);
	invalidateItemViewRendererCache();
}

void PageViewModel::initializeRenderers()
{
	AbstractViewModel::addRenderer(
		IRenderer::PlainTextRendererType | 
		IRenderer::SelectionBackgroundRendererType | 
		IRenderer::BackgroundRendererType
	);

	AbstractViewModel::setItemRendererCacheSize(s_preferredCacheSize);
}

}