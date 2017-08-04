#pragma once

namespace QuickieWebBot
{

class IGridModel : public QAbstractTableModel
{
public:
	enum CustomRoles
	{
		WhatsThisRole = Qt::UserRole,
		SelectionBackgroundColorRole,
		MarginTop,
		MarginBottom,
		MarginLeft,
		MarginRight
	};

	using QAbstractTableModel::QAbstractTableModel;
	virtual ~IGridModel() = default;
};

}
