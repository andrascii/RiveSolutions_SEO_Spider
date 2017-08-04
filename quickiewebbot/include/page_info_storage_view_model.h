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

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizeStrategy() const noexcept override;

private:
	PageInfoStorageModel* m_model;
};

}