#include "page_raw_info.h"
#include "abstract_table_model.h"
#include "default_column_resize_policy.h"
#include "url_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "grid_line_renderer.h"
#include "web_site_pages_storage_model.h"
#include "web_site_pages_storage_view_model.h"
#include "quickie_web_bot_helpers.h"
#include "table_view.h"


namespace QuickieWebBot
{

WebSitePagesStorageViewModel::WebSitePagesStorageViewModel(WebSitePagesStorageModel* model, QObject* parent)
	: AbstractViewModel(model, parent)
	, m_selectionBgColor("#C0C0C0")
	, m_hoveredBgColor("#F3F3F3")
	, m_bgColor(Qt::white)
{
	initializeRenderers();

	QPixmap pixmap;
	pixmap.fill(backgroundColor(QModelIndex()));
	pixmap.load(":/images/click-to-url-icon.png");

	m_urlIcon = pixmap.scaled(QuickieWebBotHelpers::pointsToPixels(12), QuickieWebBotHelpers::pointsToPixels(12));

	VERIFY(connect(model, SIGNAL(internalDataChanged()), this, SLOT(onAttachedModelInternalDataChanged())));
}

int WebSitePagesStorageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(4);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int WebSitePagesStorageViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int WebSitePagesStorageViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(2);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int WebSitePagesStorageViewModel::marginRight(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

const QPixmap& WebSitePagesStorageViewModel::itemPixmap(const QModelIndex& index) const noexcept
{
	const WebSitePagesStorageModel* model = 
		static_cast<const WebSitePagesStorageModel*>(AbstractViewModel::model());

	static QPixmap emptyPixmap;

	if (model->itemType(index) == PageRawInfo::UrlItemType && hoveredIndex() == index)
	{
		return m_urlIcon;
	}

	return emptyPixmap;
}

QRect WebSitePagesStorageViewModel::itemPixmapPosition(const QModelIndex&) const noexcept
{
	return QRect();
}

const QColor& WebSitePagesStorageViewModel::selectionBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBgColor;
}

const QColor& WebSitePagesStorageViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBgColor;
}

const QColor& WebSitePagesStorageViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_bgColor;
}

const QFont& WebSitePagesStorageViewModel::font(const QModelIndex& index) const noexcept
{
	static QFont font = Application::font();

	return font;
}

Qt::AlignmentFlag WebSitePagesStorageViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Qt::AlignCenter;
	}

	return Qt::AlignLeft;
}

QColor WebSitePagesStorageViewModel::textColor(const QModelIndex& index) const noexcept
{
	const WebSitePagesStorageModel* model = 
		static_cast<const WebSitePagesStorageModel*>(AbstractViewModel::model());

	if (model->itemType(index) == PageRawInfo::UrlItemType)
	{
		return QColor("#1754A8");
	}

	return QColor("#000000");
}

void WebSitePagesStorageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
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

QList<const IRenderer*> WebSitePagesStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	const WebSitePagesStorageModel* model = 
		static_cast<const WebSitePagesStorageModel*>(AbstractViewModel::model());

	const IRenderer* renderer = model->itemType(index) == PageRawInfo::UrlItemType ?
		AbstractViewModel::renderer(AbstractViewModel::UrlRendererType) :
		AbstractViewModel::renderer(AbstractViewModel::PlainTextRendererType);

	return QList<const IRenderer*>()
		<< AbstractViewModel::renderer(AbstractViewModel::BackgroundRendererType)
		<< AbstractViewModel::renderer(AbstractViewModel::SelectionBackgroundRendererType)
		<< renderer
		<< AbstractViewModel::renderer(AbstractViewModel::GridLineRendererType);
}

void WebSitePagesStorageViewModel::onAttachedModelInternalDataChanged()
{
	const WebSitePagesStorageModel* model =
		static_cast<const WebSitePagesStorageModel*>(AbstractViewModel::model());

	AbstractViewModel::renderer(AbstractViewModel::PlainTextRendererType)->setCacheSize(static_cast<int>(std::pow((model->columnCount()), 2.0)));
}

void WebSitePagesStorageViewModel::initializeRenderers()
{
	const WebSitePagesStorageModel* model =
		static_cast<const WebSitePagesStorageModel*>(AbstractViewModel::model());

	AbstractViewModel::addRenderer(AbstractViewModel::PlainTextRendererType, new TextRenderer(this, static_cast<int>(std::pow((model->columnCount()), 2.0))));
	AbstractViewModel::addRenderer(AbstractViewModel::UrlRendererType, new UrlRenderer(this, static_cast<int>(std::pow(model->columnCount(), 2.0))));
	AbstractViewModel::addRenderer(AbstractViewModel::SelectionBackgroundRendererType, new SelectionBackgroundRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::BackgroundRendererType, new BackgroundRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::GridLineRendererType, new GridLineRenderer(this, QColor("#F1F1F1")));
}

}