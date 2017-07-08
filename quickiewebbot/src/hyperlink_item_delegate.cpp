#include "hyperlink_item_delegate.h"
#include "imodel_data_accessor.h"

namespace QuickieWebBot
{


HyperlinkItemDelegate::HyperlinkItemDelegate(QWidget* parent)
	: QStyledItemDelegate(parent)
{
	// TODO: install eventfilter
}

void HyperlinkItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!(index.data(Qt::UserRole).toInt() & IModelDataAccessorItem::ItemFlagUrl))
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	QUrl url = index.data().toUrl();
	auto urlText = url.toString().toStdString();

	QStyleOptionViewItem newOption = option;
	//newOption.font.setUnderline(option.state & QStyle::State_MouseOver);
	newOption.font.setUnderline(true);

	QStyledItemDelegate::paint(painter, newOption, index);

	painter->restore();
}

bool HyperlinkItemDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (index.data(Qt::UserRole).toInt() & IModelDataAccessorItem::ItemFlagUrl)
	{
		if (event->type() == QEvent::MouseButtonRelease)
		{
			// TODO: open url
		}

	}
	
	return false;
}

}