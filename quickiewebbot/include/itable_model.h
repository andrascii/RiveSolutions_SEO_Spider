#pragma once

namespace QuickieWebBot
{

class IResizePolicy;

class ITableModel : public QAbstractTableModel
{
public:
	enum CustomRoles
	{
		SelectionBackgroundColorRole = Qt::UserRole,
		HoveredBackgroundColorRole
	};

	using QAbstractTableModel::QAbstractTableModel;
	virtual ~ITableModel() = default;

	virtual IResizePolicy* resizePolicy() const noexcept = 0;

	// signals
	virtual void internalDataChanged() = 0;
};

}
