#pragma once

#include "abstract_table_model.h"
#include "data_collection.h"
#include "isummary_data_accessor.h"

namespace QuickieWebBot
{

class IResizePolicy;
class ViewportPercentResizePolicy;
	
class SummaryModel : public AbstractTableModel
{
	Q_OBJECT

public:
	SummaryModel(QObject* parent  = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QSize span(const QModelIndex& index) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	virtual IResizePolicy* resizePolicy() const noexcept override;

	void setDataAccessor(ISummaryDataAccessor* dataAccessor) noexcept;
	ISummaryDataAccessor* dataAccessor() const noexcept;

	StorageAdaptorType storageAdaptorType(const QModelIndex& index) const noexcept;

signals:
	virtual void internalDataChanged() override;

private slots:
	void formActualUpdateDataSignal(int row, int column, Qt::ItemDataRole role);

private:
	std::shared_ptr<IResizePolicy> m_resizePolicy;
	ISummaryDataAccessor* m_dataAccessor;
};

}
