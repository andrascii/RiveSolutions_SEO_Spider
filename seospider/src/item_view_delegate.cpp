#include "item_view_delegate.h"
#include "irenderer.h"
#include "iview_model.h"
#include "page_model.h"
#include "table_view.h"
#include "model_helpers.h"

namespace SeoSpider
{

ItemViewDelegate::ItemViewDelegate(IViewModel* itemView, QObject* parent)
	: QStyledItemDelegate(parent)
	, m_itemView(itemView)
{
}

void ItemViewDelegate::setViewModel(IViewModel* itemView) noexcept
{
	m_itemView = itemView;
}

IViewModel* ItemViewDelegate::itemView() const noexcept
{
	return m_itemView;
}

void ItemViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	ASSERT(m_itemView);

	m_itemView->itemViewRenderer(index)->draw(painter, option, index);
}

bool ItemViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		auto mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() != Qt::LeftButton)
		{
			return false;
		}

		TableView* tableView = qobject_cast<TableView*>(parent());

		if(!tableView)
		{
			return false;
		}

		PageModel* pageModel = getUnderlyingModelByIndex<PageModel*, false>(index);

		if (pageModel && pageModel->itemType(index) == IStorageAdapter::ItemType::UrlItemType)
		{					
			if(m_itemView->pixmapPosition(index, option.rect).contains(mouseEvent->pos()))
			{
				QVariant itemData = index.data();
				DEBUG_ASSERT(itemData.type() == QVariant::Url);
				QDesktopServices::openUrl(itemData.toUrl());
			}
			return true;
		}
	}

	return false;
}

}
