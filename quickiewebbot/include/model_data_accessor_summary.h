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

	virtual int columnCount() const override;
	virtual int rowCount() const override;

	virtual QString headerData(int column, Qt::Orientation orientation) const override;

	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual QVariant itemValue(int row, int column) const override;

	virtual PageInfoPtr pageInfoAtRow(int row) const override;

	virtual QColor itemBackgroundColor(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;
	virtual int flags(const QModelIndex& index) const override;
	virtual QPixmap* pixmap(const QModelIndex& index) const override;
	virtual QObject* qobject() override;

	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
	virtual IGridViewResizeStrategy* resizeStrategy() const override;

	virtual std::vector<const GridViewPainter*> painters(const QModelIndex& index) const override;
	virtual std::vector<GridViewPainter*> painters(const QModelIndex& index) override;

	virtual const GridViewPainter* backgroundPainter(const QModelIndex& index) const override;
	virtual GridViewPainter* backgroundPainter(const QModelIndex& index) override;

	virtual const GridViewPainter* textPainter(const QModelIndex& index) const override;
	virtual GridViewPainter* textPainter(const QModelIndex& index) override;

	Q_SIGNAL virtual void itemChanged(int row, int column) override;
	Q_SIGNAL virtual void rowRemoved(int row) override;
	Q_SIGNAL virtual void rowAdded(int row) override;
	Q_SIGNAL virtual void reset() override;

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
};

}