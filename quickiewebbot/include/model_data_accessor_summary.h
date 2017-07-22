#pragma once

#include "imodel_data_accessor.h"

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
	virtual QString columnText(int column) const override;

	virtual int rowCount() const override;
	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual QColor itemBackgroundColor(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;
	virtual int flags(const QModelIndex& index) const override;

	virtual QPixmap* pixmap(const QModelIndex& index) const override;

	virtual QObject* qobject() override;

	// signals
	Q_SIGNAL void itemChanged(QModelIndex index);
	Q_SIGNAL void rowRemoved(int row);
	Q_SIGNAL void rowAdded(int row);
	Q_SIGNAL void reset();

	virtual std::vector<GridViewPainter*> painters(const QModelIndex& index) const override;

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
		QByteArray id;
		ItemStatus status;
		int issueCount;
	};

	struct SummaryGroup 
	{
		QString groupName;
		std::vector<SummaryItem> groupItems;
	};

	bool isGroupHeaderRow(int row) const;
	QPixmap getPixmapIcon(ItemStatus status, int size) const;

private:
	std::vector<SummaryGroup> m_groups;
	std::map<int, SummaryGroup*> m_groupRefs;
	std::map<int, SummaryItem*> m_itemRefs;

};

}