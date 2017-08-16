#pragma once

namespace QuickieWebBot
{

class TableView;

class Delegate : public QStyledItemDelegate
{
public:
	Delegate(TableView* parent = nullptr);

protected:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
	TableView* m_view;
};

}