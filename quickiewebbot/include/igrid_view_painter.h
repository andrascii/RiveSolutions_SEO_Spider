#pragma once


namespace QuickieWebBot
{

class IGridViewPainter
{
public:
	virtual ~IGridViewPainter() = default;
	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const = 0;
};

}