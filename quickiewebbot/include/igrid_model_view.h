#pragma once

namespace QuickieWebBot
{

class IGridViewPainter;
class IGridViewResizeStrategy;

class IGridModelView
{
public:
	virtual const QFont& textFont(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QPixmap& decoration(const QModelIndex& index) const noexcept = 0;
	virtual QList<IGridViewPainter*> painters(const QModelIndex& index) const noexcept = 0;
	virtual IGridViewResizeStrategy* resizeStrategy() const noexcept = 0;

	// signals
	virtual void textFontChanged(const QModelIndex& index) = 0;
	virtual void textColorChanged(const QModelIndex& index) = 0;
	virtual void backgroundColorChanged(const QModelIndex& index) = 0;
	virtual void selectionBackgroundColorChanged(const QModelIndex& index) = 0;
	virtual void decorationChanged(const QModelIndex& index) = 0;
};

}
