#pragma once


namespace QuickieWebBot
{

class IGridViewPainter
{
public:
	virtual ~IGridViewPainter() = default;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;
};

}