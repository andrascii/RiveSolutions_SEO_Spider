#pragma once

#include "isummary_data_accessor.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class SummaryFilterDataAccessor : public ISummaryDataAccessor
{
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
		SummaryCategoryItem category;
		ItemStatus status;
		int issueCount;
		int row;
	};

	struct SummaryGroup
	{
		QString groupName;
		std::vector<SummaryItem> groupItems;
	};

public:
	SummaryFilterDataAccessor();

	virtual int columnCount() const noexcept override;
	virtual int rowCount() const noexcept override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept override;
	virtual QSize span(const QModelIndex& index) const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual bool isHeaderItem(const QModelIndex& index) const noexcept override;
	virtual SummaryCategoryItem itemCategory(const QModelIndex& index) const noexcept override;
	virtual QPixmap pixmap(const QModelIndex& index) const noexcept override;

private:
	ItemStatus itemStatus(const QModelIndex& index) const noexcept;

private:
	static constexpr int s_summaryColumnCount = 2;

	QVector<SummaryGroup> m_allGroups;
	QMap<int, SummaryGroup*> m_groups;
	QMap<int, SummaryItem*> m_itemRows;
	QMap<SummaryCategoryItem, SummaryItem*> m_itemTypes;
};

}