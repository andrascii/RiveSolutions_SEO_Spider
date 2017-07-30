#pragma once

#include "grid_model.h"
#include "igrid_view_resize_strategy.h"

namespace QuickieWebBot
{
	
class SummaryModel : public QAbstractTableModel
{
public:
	SummaryModel(QObject* parent = nullptr);

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

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

	bool isGroupHeaderRow(int row) const;
	QPixmap pixmapIcon(ItemStatus status, int size) const;

private:
	QVector<SummaryGroup> m_groups;
	QMap<int, SummaryGroup*> m_groupByRowRefs;
	QMap<int, SummaryItem*> m_itemByRowRefs;
	QMap<int, SummaryItem*> m_itemByTypeRefs;

	std::unique_ptr<IGridViewResizeStrategy> m_resizeStrategy;
};

}
