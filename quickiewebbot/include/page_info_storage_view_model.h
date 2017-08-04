#pragma once

#include "igrid_view_model.h"

namespace QuickieWebBot
{

class PageInfoStorageModel;

class PageInfoStorageViewModel 
	: public QObject
	, public IGridViewModel
{
	Q_OBJECT

public:
	PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent = nullptr);

	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizePolicy() const noexcept override;

private:
	PageInfoStorageModel* m_model;

	std::unique_ptr<GridViewResizePolicy> m_resizePolicy;
};

}