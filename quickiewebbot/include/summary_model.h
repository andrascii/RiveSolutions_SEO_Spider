#pragma once

#include "igrid_model.h"

namespace QuickieWebBot
{

class IGridViewResizePolicy;
class GridViewFullSizeResizeStrategy;
	
class SummaryModel : public IGridModel
{
	Q_OBJECT

public:
	SummaryModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual QSize span(const QModelIndex& index) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

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

	QVector<SummaryGroup> m_allGroups;
	QMap<int, SummaryGroup*> m_groups;
	QMap<int, SummaryItem*> m_itemRows;
	QMap<int, SummaryItem*> m_itemTypes;

	std::unique_ptr<GridViewFullSizeResizeStrategy> m_resizeStrategy;
};

}
