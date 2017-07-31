#pragma once

#include "igrid_model.h"

namespace QuickieWebBot
{

class IGridViewResizeStrategy;
class GridViewResizeStrategy;

class PageInfoStorageModel : public IGridModel
{
	Q_OBJECT

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
	std::unique_ptr<GridViewResizeStrategy> m_resizeStrategy;
};

}
