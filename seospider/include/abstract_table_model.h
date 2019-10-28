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
	virtual void storageAdapterChanged() = 0;

	static constexpr int underlyingIndexRole = Qt::UserRole;
	static constexpr int resourceTypeRole = underlyingIndexRole - 1;
	static constexpr int columnEnabledRole = underlyingIndexRole - 2;
	static constexpr int canonicalUrlRole = underlyingIndexRole - 3;
};



}
