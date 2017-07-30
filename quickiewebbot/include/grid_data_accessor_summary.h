#pragma once

#include "grid_data_accessor_base.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

class GridDataAccessorSummary 
	: public QObject
	, public GridDataAccessorBase
{
	Q_OBJECT

public:
	GridDataAccessorSummary();

	virtual int columnsCount() const override;
	virtual int itemCount() const override;

	virtual QVariant columnDescription(int section) const override;

	virtual QVariant item(const QModelIndex& index) const override;
	virtual QVariant item(int row, int column) const override;

	virtual PageInfoPtr pageInfoAtRow(int row) const override;

	virtual GridDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
	virtual IGridViewResizeStrategy* resizeStrategy() const override;

	virtual QObject* qobject() override;

signals:
	virtual void itemChanged(int row, int column) override;
	virtual void rowRemoved(int row) override;
	virtual void rowAdded(int row) override;
	virtual void reset() override;

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
	Q_SLOT void onModelDataRowAdded(int row, int type);

private:
	QVector<SummaryGroup> m_groups;
	QMap<int, SummaryGroup*> m_groupByRowRefs;
	QMap<int, SummaryItem*> m_itemByRowRefs;
	QMap<int, SummaryItem*> m_itemByTypeRefs;

	std::unique_ptr<IGridViewResizeStrategy> m_resizeStrategy;

	GridViewPainterText m_textPainter;
	GridViewPainterBackground m_backgroundPainter;
};

}