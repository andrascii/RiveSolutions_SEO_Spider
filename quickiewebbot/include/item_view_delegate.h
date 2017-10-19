#pragma once

namespace QuickieWebBot
{

class IViewModel;
class TableView;

class ItemViewDelegate : public QStyledItemDelegate
{
public:
	ItemViewDelegate(IViewModel* itemView, QObject* parent = nullptr);

	void setViewModel(IViewModel* itemView) noexcept;
	IViewModel* itemView() const noexcept;

protected:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
	IViewModel* m_itemView;
};

}