#include "abstract_table_model.h"
#include "page_model.h"
#include "page_view_model.h"
#include "helpers.h"
#include "table_view.h"
#include "url.h"

namespace
{

constexpr int s_preferredCacheSize = 500;

}

namespace SeoSpider
{

PageViewModel::PageViewModel(IView* view, PageModel* model, QObject* parent)
	: AbstractViewModel(model, parent)
	, m_view(view)
	, m_selectionBackgroundColor("#F7F0D6")
	, m_hoveredBackgroundColor("#F2F2F2")
	, m_backgroundColor("#FCFDFE")
	, m_selectedGridLineColor("#E8E0C1")
	, m_gridLineColor("#F3F3F3")
	, m_urlTextColor("#1C51AF")
	, m_textColor("#000000")
	, m_textFont("Helvetica", 10, QFont::Normal)
	, m_itemRenderer(this)
{
	initializeRenderers();

	QPixmap pixmap;
	pixmap.fill(m_backgroundColor);
	pixmap.load(":/images/click-to-url-icon.png");

	m_urlIcon = pixmap.scaled(Common::Helpers::pointsToPixels(13), Common::Helpers::pointsToPixels(13));

	VERIFY(connect(model, &PageModel::internalDataChanged, this, &PageViewModel::onAttachedModelInternalDataChanged));
	VERIFY(connect(model, &PageModel::modelReset, this, &PageViewModel::onModelDataWasReset));
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

const QPixmap& PageViewModel::pixmap(const QModelIndex& index) const noexcept
{
	const PageModel* model = 
		static_cast<const PageModel*>(AbstractViewModel::model());

	static QPixmap emptyPixmap;

	if (model->itemType(index) == IStorageAdapter::ItemType::UrlItemType && 
		hoveredIndex().row() == index.row() &&
		CrawlerEngine::Url(index.data(Qt::DisplayRole).toString()).isValid())
	{
		return m_urlIcon;
	}

	return emptyPixmap;
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
	DEBUG_ASSERT(index.model() == static_cast<const QAbstractItemModel*>(model()));

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
		modelIndexes.append(model()->makeModelIndexesForRow(previousHoveredIndex.row()));
		modelIndexes.append(model()->makeModelIndexesForRow(previousHoveredIndex.row() - 1));
		
		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);

		AbstractViewModel::resetPreviousHoveredIndex();
	}

	if (hoveredIndex().isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(model()->makeModelIndexesForRow(hoveredIndex().row()));
		modelIndexes.append(model()->makeModelIndexesForRow(hoveredIndex().row() - 1));

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}

void PageViewModel::onAttachedModelInternalDataChanged()
{
	const PageModel* model =
		static_cast<const PageModel*>(AbstractViewModel::model());

	AbstractViewModel::clearSelectedIndexes();

	invalidateItemViewRendererCache();

	AbstractViewModel::setItemRendererCacheSize(static_cast<int>(model->columnCount() * model->columnCount()));
}

void PageViewModel::onModelDataWasReset()
{
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