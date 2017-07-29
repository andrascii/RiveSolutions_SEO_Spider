#pragma once

#include "model_data_accessor_base.h"
#include "grid_view_painter_text.h"
#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

class ModelDataAccessorSummary 
	: public QObject
	, public ModelDataAccessorBase
{
	Q_OBJECT

public:
	ModelDataAccessorSummary();

	virtual QList<PageInfo::ItemType> supportedColumns() const override;
	virtual int itemCount() const override;

	virtual QVariant item(const QModelIndex& index) const override;
	virtual QVariant item(int row, int column) const override;

	virtual PageInfoPtr pageInfoAtRow(int row) const override;

	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
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
	std::vector<SummaryGroup> m_groups;
	std::map<int, SummaryGroup*> m_groupByRowRefs;
	std::map<int, SummaryItem*> m_itemByRowRefs;
	std::map<int, SummaryItem*> m_itemByTypeRefs;

	std::unique_ptr<IGridViewResizeStrategy> m_resizeStrategy;

	GridViewPainterText m_textPainter;
	GridViewPainterBackground m_backgroundPainter;

	QList<PageInfo::ItemType> m_supportedColumns;
};

}