#pragma once

namespace QuickieWebBot
{

class TableView;

class ItemViewDelegate : public QStyledItemDelegate
{
public:
	ItemViewDelegate(IViewModel* viewModel, QObject* parent = nullptr);

	void setViewModel(IViewModel* viewModel) noexcept;
	IViewModel* viewModel() const noexcept;

protected:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
	IViewModel* m_viewModel;
};

}