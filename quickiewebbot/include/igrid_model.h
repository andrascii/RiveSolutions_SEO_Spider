#pragma once

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	enum CustomRoles
	{
		WhatsThisRole = Qt::UserRole
	};

	using QAbstractTableModel::QAbstractTableModel;
	virtual ~IGridModel() = default;
};

}
