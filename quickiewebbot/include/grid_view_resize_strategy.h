#pragma once
#include "igrid_view_resize_strategy.h"

namespace QuickieWebBot
{

class GridViewResizeStrategy : public IGridViewResizeStrategy
{
public:
	GridViewResizeStrategy();
	virtual void resize(GridView* gridView) const override;
	virtual void init(GridView* gridView, const IGridViewResizeStrategy* prev = nullptr) override;
	virtual int columnSize(int column, const GridView* gridView) const override;

	void setColumnsSize(std::map<int, int> size);
private:
	std::map<int, int> m_columnsSize;
};

}