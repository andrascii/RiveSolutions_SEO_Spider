#pragma once

#include "abstract_table_model.h"
#include "istorage_adapter.h"

namespace SeoSpider
{

class IResizePolicy;

class PageModel : public AbstractTableModel
{
	Q_OBJECT

public:
	PageModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual IResizePolicy* resizePolicy() const noexcept override;
	void setStorageAdapter(IStorageAdapter* storageAdapter) noexcept;
	const IStorageAdapter* storageAdapter() const;
	IStorageAdapter* storageAdapter();
	IStorageAdapter::ItemType itemType(const QModelIndex& index) const noexcept;

signals:
	virtual void internalDataChanged() override;

private slots:
	void onParsedPageInfoAdded(int rowIndex);
	void onParsedPageInfoRemoved(int rowIndex);
	void onParsedPageInfoReplaced(int rowIndex);
	void onRepaintIndicesRange(std::pair<int, int> indicesRange);
	void onAboutBeginClearingData();
	void onAboutEndClearingData();

private:
	IStorageAdapter* m_storageAdapter;
	std::shared_ptr<IResizePolicy> m_resizePolicy;
};

}
