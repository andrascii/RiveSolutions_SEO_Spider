#pragma once

#include "igrid_data_accessor.h"

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	virtual IGridViewResizeStrategy* resizeStrategy() const = 0;
};

}
