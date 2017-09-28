#pragma once

namespace QuickieWebBot
{

class IResizePolicy;

class AbstractTableModel : public QAbstractTableModel
{
public:
	using QAbstractTableModel::QAbstractTableModel;
	virtual ~AbstractTableModel() = default;

	virtual IResizePolicy* resizePolicy() const noexcept = 0;

	// signals
	virtual void internalDataChanged() = 0;

	QModelIndexList makeModelIndexesForRow(int row) const noexcept;
};

}
