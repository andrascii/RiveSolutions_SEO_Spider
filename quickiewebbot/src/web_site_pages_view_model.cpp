#include "page_raw_info.h"
#include "abstract_table_model.h"
#include "default_column_resize_policy.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "grid_line_renderer.h"
#include "web_site_pages_model.h"
#include "web_site_pages_view_model.h"
#include "quickie_web_bot_helpers.h"
#include "table_view.h"


namespace QuickieWebBot
{

WebSitePagesViewModel::WebSitePagesViewModel(WebSitePagesModel* model, QObject* parent)
	: AbstractViewModel(model, parent)
	, m_selectionBgColor("#E6EE9C")
	, m_hoveredBgColor("#F3F3F3")
	, m_bgColor(Qt::white)
	, m_itemRenderer(this)
{
	initializeRenderers();

	QPixmap pixmap;
	pixmap.fill(backgroundColor(QModelIndex()));
	pixmap.load(":/images/click-to-url-icon.png");

	m_urlIcon = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(13), QuickieWebBotHelpers::pointsToPixels(13));

	VERIFY(connect(model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));
}

int WebSitePagesViewModel::marginTop(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(4);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int WebSitePagesViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int WebSitePagesViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(2);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int WebSitePagesViewModel::marginRight(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

const QPixmap& WebSitePagesViewModel::pixmap(const QModelIndex& index) const noexcept
{
	const WebSitePagesModel* model = 
		static_cast<const WebSitePagesModel*>(AbstractViewModel::model());

	static QPixmap emptyPixmap;

	if (model->itemType(index) == PageRawInfo::UrlItemType && hoveredIndex().row() == index.row())
	{
		return m_urlIcon;
	}

	return emptyPixmap;
}

QRect WebSitePagesViewModel::pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	const WebSitePagesModel* model =
		static_cast<const WebSitePagesModel*>(AbstractViewModel::model());

	if (model->itemType(index) == PageRawInfo::UrlItemType)
	{
		return itemVisualRect.adjusted(itemVisualRect.width() - QuickieWebBotHelpers::pointsToPixels(20), 0, 0, 0);
	}

	return itemVisualRect.adjusted(itemVisualRect.width() + QuickieWebBotHelpers::pointsToPixels(3), 0, 0, 0);
}

QString WebSitePagesViewModel::displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept
{
	DEBUG_ASSERT(index.model() == static_cast<const QAbstractItemModel*>(model()));

	const QString displayData = index.data(Qt::DisplayRole).toString();

	const int pixmapOccupiedWidth = index.row() == hoveredIndex().row() ? itemVisualRect.width() - pixmapPosition(index, itemVisualRect).x() : 0;

	QFontMetrics fontMetrics(Application::font());
	return fontMetrics.elidedText(displayData, Qt::ElideRight, itemVisualRect.width() - pixmapOccupiedWidth);
}

QRect WebSitePagesViewModel::displayDataPosition(const QModelIndex&, const QRect& itemVisualRect) const noexcept
{
	return itemVisualRect;
}

const QColor& WebSitePagesViewModel::selectionBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBgColor;
}

const QColor& WebSitePagesViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBgColor;
}

const QColor& WebSitePagesViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_bgColor;
}

const QFont& WebSitePagesViewModel::font(const QModelIndex& index) const noexcept
{
	static QFont font = Application::font();

	return font;
}

Qt::AlignmentFlag WebSitePagesViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Qt::AlignCenter;
	}

	return Qt::AlignLeft;
}

QColor WebSitePagesViewModel::textColor(const QModelIndex& index) const noexcept
{
	const WebSitePagesModel* model = 
		static_cast<const WebSitePagesModel*>(AbstractViewModel::model());

	if (model->itemType(index) == PageRawInfo::UrlItemType)
	{
		return QColor("#1754A8");
	}

	return QColor("#000000");
}

void WebSitePagesViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	AbstractViewModel::setHoveredIndex(index);

	QModelIndex previousHoveredIndex = AbstractViewModel::previousHoveredIndex();

	if (previousHoveredIndex.isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(model()->modelIndexesForRow(previousHoveredIndex.row()));
		modelIndexes.append(model()->modelIndexesForRow(previousHoveredIndex.row() - 1));
		
		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}

	if (hoveredIndex().isValid())
	{
		QModelIndexList modelIndexes;
		modelIndexes.append(model()->modelIndexesForRow(hoveredIndex().row()));
		modelIndexes.append(model()->modelIndexesForRow(hoveredIndex().row() - 1));

		AbstractViewModel::emitNeedToRepaintIndexes(modelIndexes);
	}
}

void WebSitePagesViewModel::onAttachedModelInternalDataChanged()
{
	const WebSitePagesModel* model =
		static_cast<const WebSitePagesModel*>(AbstractViewModel::model());

	AbstractViewModel::clearSelectedIndexes();

	invalidateItemViewRendererCache();

	AbstractViewModel::setItemRendererCacheSize(static_cast<int>(model->columnCount() * model->rowCount() * 2));
}

void WebSitePagesViewModel::initializeRenderers()
{
	const WebSitePagesModel* model =
		static_cast<const WebSitePagesModel*>(AbstractViewModel::model());

	AbstractViewModel::addRenderer(
		IRenderer::PlainTextRendererType | 
		IRenderer::SelectionBackgroundRendererType | 
		IRenderer::BackgroundRendererType | 
		IRenderer::GridLineRendererType
	);

	AbstractViewModel::setItemRendererCacheSize(static_cast<int>(model->columnCount() * model->rowCount() * 2));
}

}