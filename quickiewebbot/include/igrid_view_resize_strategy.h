#pragma once

namespace QuickieWebBot
{

class GridView;

class IGridViewResizeStrategy
{
public:
	virtual ~IGridViewResizeStrategy() = default;
	virtual void resize(GridView* gridView) const = 0;

};

}