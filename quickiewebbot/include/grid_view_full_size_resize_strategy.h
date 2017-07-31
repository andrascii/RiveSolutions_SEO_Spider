#pragma once
#include "igrid_view_resize_strategy.h"

namespace QuickieWebBot
{

class GridViewFullSizeResizeStrategy : public IGridViewResizeStrategy
{
public:
	GridViewFullSizeResizeStrategy(std::vector<int> columnsPercentSize);
	virtual void resize(GridView* gridView) const noexcept override;
	virtual void init(GridView* gridView, const IGridViewResizeStrategy* prev = nullptr) const noexcept override;
	virtual int columnSize(int column, const GridView* gridView) const noexcept override;

private:
	std::vector<int> m_columnsPercentSize;
};

}