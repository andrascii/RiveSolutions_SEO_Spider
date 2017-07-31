#pragma once

#include "igrid_model.h"
#include "page_info.h"

namespace QuickieWebBot
{

class IGridViewResizeStrategy;
class GridViewResizeStrategy;

//
//! This model intended for establish associate with some PageInfo storage 
//! which placed in the DataCollection instance.
//! Using this model you can customize PageInfo columns set.
//

class PageInfoStorageModel : public IGridModel
{
	Q_OBJECT

	//
	// If you want to add a model for only one PageInfo then you need
	// to create a new derived type from IGridModel and customize it appropriately.
	//

public:
	PageInfoStorageModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	//
	// IGridModel implementation
	//
	virtual IGridViewResizeStrategy* resizeStrategy() const override;

private:
	QList<PageInfo::ItemType> m_pageInfoColumns;

	std::unique_ptr<GridViewResizeStrategy> m_resizeStrategy;
};

}
