#pragma once

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	enum CustomRoles
	{
		WhatsThisRole = Qt::UserRole,
		SelectionBackgroundColorRole
	};

	using QAbstractTableModel::QAbstractTableModel;
	virtual ~IGridModel() = default;
};

}
