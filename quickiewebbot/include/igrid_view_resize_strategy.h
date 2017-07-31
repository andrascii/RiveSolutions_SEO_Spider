#pragma once

namespace QuickieWebBot
{

class GridView;

class IGridViewResizeStrategy
{
public:
	virtual ~IGridViewResizeStrategy() = default;

	virtual void init(GridView* gridView, const IGridViewResizeStrategy* prev = nullptr) const noexcept = 0;

	virtual void resize(GridView* gridView) const noexcept = 0;
	virtual int columnSize(int column, const GridView* gridView) const noexcept = 0;
};

}