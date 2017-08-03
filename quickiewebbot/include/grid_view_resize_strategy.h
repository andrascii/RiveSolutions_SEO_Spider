#pragma once

#include "igrid_view_resize_strategy.h"

namespace QuickieWebBot
{

class GridViewResizeStrategy : public IGridViewResizePolicy
{
public:
	GridViewResizeStrategy();

	virtual void init(GridView* gridView, const IGridViewResizePolicy* prev = nullptr)  const noexcept override;

	virtual void resize(GridView* gridView) const noexcept override;
	virtual int columnSize(int column, const GridView* gridView) const noexcept override;

	void setColumnsSize(std::map<int, int> size);

private:
	std::map<int, int> m_columnsSize;
};

}