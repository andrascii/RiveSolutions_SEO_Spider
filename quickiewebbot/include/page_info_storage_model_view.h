#pragma once

#include "igrid_model_view.h"

namespace QuickieWebBot
{

class PageInfoStorageModel;

class PageInfoStorageModelView 
	: public QObject
	, public IGridModelView
{
	Q_OBJECT

public:
	PageInfoStorageModelView(PageInfoStorageModel* model, QObject* parent = nullptr);

	virtual const QFont& textFont(const QModelIndex& index) const noexcept override;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QPixmap& decoration(const QModelIndex& index) const noexcept override;
	virtual QList<IGridViewPainter*> painters(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizeStrategy* resizeStrategy() const noexcept override;

signals:
	void textFontChanged(const QModelIndex& index);
	void textColorChanged(const QModelIndex& index);
	void backgroundColorChanged(const QModelIndex& index);
	void selectionBackgroundColorChanged(const QModelIndex& index);
	void decorationChanged(const QModelIndex& index);

private:
	PageInfoStorageModel* m_model;
};

}