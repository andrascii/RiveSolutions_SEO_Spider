#pragma once

namespace SeoSpider
{

class IResizePolicy;

class AbstractTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	using QAbstractTableModel::QAbstractTableModel;
	virtual ~AbstractTableModel() = default;

	virtual IResizePolicy* resizePolicy() const noexcept = 0;

	// signals
	virtual void internalDataChanged() = 0;

	static constexpr int underlyingIndexRole = Qt::UserRole;
};



}
