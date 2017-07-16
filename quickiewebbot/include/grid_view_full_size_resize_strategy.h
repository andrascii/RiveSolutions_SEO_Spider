#pragma once
#include "igrid_view_resize_strategy.h"

namespace QuickieWebBot
{

class GridViewFullSizeResizeStrategy : public IGridViewResizeStrategy
{
public:
	GridViewFullSizeResizeStrategy(std::vector<int> columnsPercentSize);
	virtual void resize(GridView* gridView) const override;

private:
	std::vector<int> m_columnsPercentSize;
};

}