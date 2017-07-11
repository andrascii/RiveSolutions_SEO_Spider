#pragma once

namespace QuickieWebBot
{

class GridView;

class GridViewDelegate : public QStyledItemDelegate
{
public:
	GridViewDelegate(GridView* parent = nullptr);

protected:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
	GridView* m_gridView;
};

}