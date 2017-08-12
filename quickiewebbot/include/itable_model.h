#pragma once

namespace QuickieWebBot
{

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
};

}
