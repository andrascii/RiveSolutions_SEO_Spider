#pragma once

#include "igrid_view_model.h"

namespace QuickieWebBot
{

class SummaryModel;
class GridViewFullSizeResizeStrategy;

class SummaryViewModel
	: public QObject
	, public IGridViewModel
{
	Q_OBJECT

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizeStrategy() const noexcept override;

private:
	SummaryModel* m_model;

	std::unique_ptr<GridViewFullSizeResizeStrategy> m_resizeStrategy;
};

}
