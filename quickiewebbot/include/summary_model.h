#pragma once

#include "itable_model.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class IResizePolicy;
class ViewportPercentResizePolicy;
	
class SummaryModel : public ITableModel
{
	Q_OBJECT

public:
	SummaryModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual QSize span(const QModelIndex& index) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	virtual IResizePolicy* resizePolicy() const noexcept override;
	WebCrawler::DataCollection::StorageType itemType(const QModelIndex& index) const noexcept;

signals:
	virtual void internalDataChanged() override;

private:
	enum ItemStatus
	{
		StatusOK,
		StatusWarning,
		StatusError
	};

	struct SummaryItem
	{
		QString name;
		int id;
		ItemStatus status;
		int issueCount;
		int row;
	};

	struct SummaryGroup
	{
		QString groupName;
		std::vector<SummaryItem> groupItems;
	};

private:
	bool isGroupHeaderRow(int row) const;
	QPixmap pixmap(ItemStatus status, int size) const;

private:
	static constexpr int s_summaryColumnCount = 2;

	std::shared_ptr<IResizePolicy> m_resizePolicy;

	QVector<SummaryGroup> m_allGroups;
	QMap<int, SummaryGroup*> m_groups;
	QMap<int, SummaryItem*> m_itemRows;
	QMap<int, SummaryItem*> m_itemTypes;
};

}
