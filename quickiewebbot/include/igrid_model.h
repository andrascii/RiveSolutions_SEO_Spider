#pragma once

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	//
	// ctor inheritance
	//
	using QAbstractTableModel::QAbstractTableModel;

	virtual ~IGridModel() = default;

	virtual IGridViewResizeStrategy* resizeStrategy() const = 0;
};

}
