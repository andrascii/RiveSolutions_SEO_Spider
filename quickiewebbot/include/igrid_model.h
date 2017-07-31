#pragma once

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	virtual IGridViewResizeStrategy* resizeStrategy() const = 0;
};

}
