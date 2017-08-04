#pragma once

#include "igrid_view_model.h"

namespace QuickieWebBot
{

class SummaryModel;

class SummaryViewModel
	: public QObject
	, public IGridViewModel
{
	Q_OBJECT

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizeStrategy() const noexcept override;

private:
	SummaryModel* m_model;
};

}
