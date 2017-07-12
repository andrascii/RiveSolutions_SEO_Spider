#pragma once


namespace QuickieWebBot
{

class GridViewPainter
{
public:
	virtual ~GridViewPainter() = default;

	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const = 0;
};

}