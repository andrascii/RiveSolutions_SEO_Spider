#include "page_info.h"
#include "abstract_table_model.h"
#include "default_column_resize_policy.h"
#include "url_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "grid_line_renderer.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "quickie_web_bot_helpers.h"
#include "table_view.h"


namespace QuickieWebBot
{

PageInfoStorageViewModel::PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent)
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

int PageInfoStorageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(4);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int PageInfoStorageViewModel::marginBottom(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return QuickieWebBotHelpers::pointsToPixels(2);
	}

	return QuickieWebBotHelpers::pointsToPixels(6);
}

int PageInfoStorageViewModel::marginRight(const QModelIndex&) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

const QPixmap& PageInfoStorageViewModel::itemPixmap(const QModelIndex& index) const noexcept
{
	const PageInfoStorageModel* model = 
		static_cast<const PageInfoStorageModel*>(AbstractViewModel::model());

	static QPixmap emptyPixmap;

	if (model->itemType(index) == PageInfo::UrlItemType && hoveredIndex() == index)
	{
		return m_urlIcon;
	}

	return emptyPixmap;
}

QRect PageInfoStorageViewModel::itemPixmapPosition(const QModelIndex&) const noexcept
{
	return QRect();
}

const QColor& PageInfoStorageViewModel::selectionBackgroundColor(const QModelIndex&) const noexcept
{
	return m_selectionBgColor;
}

const QColor& PageInfoStorageViewModel::hoveredBackgroundColor(const QModelIndex&) const noexcept
{
	return m_hoveredBgColor;
}

const QColor& PageInfoStorageViewModel::backgroundColor(const QModelIndex&) const noexcept
{
	return m_bgColor;
}

const QFont& PageInfoStorageViewModel::font(const QModelIndex& index) const noexcept
{
	static QFont font = Application::font();

	return font;
}

Qt::AlignmentFlag PageInfoStorageViewModel::textAlignment(const QModelIndex& index) const noexcept
{
	if (index.column() == 0)
	{
		return Qt::AlignCenter;
	}

	return Qt::AlignLeft;
}

QColor PageInfoStorageViewModel::textColor(const QModelIndex& index) const noexcept
{
	const PageInfoStorageModel* model = 
		static_cast<const PageInfoStorageModel*>(AbstractViewModel::model());

	if (model->itemType(index) == PageInfo::UrlItemType)
	{
		return QColor("#1754A8");
	}

	return QColor("#000000");
}

void PageInfoStorageViewModel::setHoveredIndex(const QModelIndex& index) noexcept
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

QList<const IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	const PageInfoStorageModel* model = 
		static_cast<const PageInfoStorageModel*>(AbstractViewModel::model());

	const IRenderer* renderer = model->itemType(index) == PageInfo::UrlItemType ?
		AbstractViewModel::renderer(AbstractViewModel::UrlRendererType) :
		AbstractViewModel::renderer(AbstractViewModel::PlainTextRendererType);

	return QList<const IRenderer*>()
		<< AbstractViewModel::renderer(AbstractViewModel::BackgroundRendererType)
		<< AbstractViewModel::renderer(AbstractViewModel::SelectionBackgroundRendererType)
		<< renderer
		<< AbstractViewModel::renderer(AbstractViewModel::GridLineRendererType);
}

void PageInfoStorageViewModel::onAttachedModelInternalDataChanged()
{
	const PageInfoStorageModel* model =
		static_cast<const PageInfoStorageModel*>(AbstractViewModel::model());

	AbstractViewModel::renderer(AbstractViewModel::PlainTextRendererType)->setCacheSize(static_cast<int>(std::pow((model->columnCount()), 2.0)));
}

void PageInfoStorageViewModel::initializeRenderers()
{
	const PageInfoStorageModel* model =
		static_cast<const PageInfoStorageModel*>(AbstractViewModel::model());

	AbstractViewModel::addRenderer(AbstractViewModel::PlainTextRendererType, new TextRenderer(this, static_cast<int>(std::pow((model->columnCount()), 2.0))));
	AbstractViewModel::addRenderer(AbstractViewModel::UrlRendererType, new UrlRenderer(this, static_cast<int>(std::pow(model->columnCount(), 2.0))));
	AbstractViewModel::addRenderer(AbstractViewModel::SelectionBackgroundRendererType, new SelectionBackgroundRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::BackgroundRendererType, new BackgroundRenderer(this));
	AbstractViewModel::addRenderer(AbstractViewModel::GridLineRendererType, new GridLineRenderer(this, QColor("#F1F1F1")));
}

}