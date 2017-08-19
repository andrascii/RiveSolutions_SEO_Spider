#pragma once

#include "iview_model.h"

namespace QuickieWebBot
{

class SummaryModel;
class ViewportPercentResizePolicy;

class SummaryViewModel
	: public QObject
	, public IViewModel
{
	Q_OBJECT

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual void resetRenderersCache() const noexcept override;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;

private:
	SummaryModel* m_model;

	std::unique_ptr<ViewportPercentResizePolicy> m_resizePolicy;
};

}
