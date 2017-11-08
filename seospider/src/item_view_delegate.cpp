#include "item_view_delegate.h"
#include "irenderer.h"
#include "iview_model.h"

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
	return false;
}

}