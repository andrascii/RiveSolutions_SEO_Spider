#pragma once

#include <QStyledItemDelegate>

namespace QuickieWebBot
{

class HyperlinkItemDelegate : public QStyledItemDelegate
{
public:
	HyperlinkItemDelegate(QWidget* parent = nullptr);

protected:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index) override;
};

}