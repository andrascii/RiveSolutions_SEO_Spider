#pragma once

namespace QuickieWebBot
{

class IResizePolicy;

class ITableModel : public QAbstractTableModel
{
public:
	enum CustomRoles
	{
		WhatsThisRole = Qt::UserRole,
		SelectionBackgroundColorRole,
		HoveredBackgroundColorRole,
		MarginTop,
		MarginBottom,
		MarginLeft,
		MarginRight
	};

	using QAbstractTableModel::QAbstractTableModel;
	virtual ~ITableModel() = default;

	virtual IResizePolicy* resizePolicy() const noexcept = 0;
};

}
