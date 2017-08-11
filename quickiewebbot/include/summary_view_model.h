#pragma once

#include "igrid_view_model.h"

namespace QuickieWebBot
{

class SummaryModel;
class GridViewFullSizeResizePolicy;

class SummaryViewModel
	: public QObject
	, public IGridViewModel
{
	Q_OBJECT

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual void resetRenderersCache() const noexcept override;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizePolicy() const noexcept override;

private:
	SummaryModel* m_model;

	std::unique_ptr<GridViewFullSizeResizePolicy> m_resizePolicy;
};

}
