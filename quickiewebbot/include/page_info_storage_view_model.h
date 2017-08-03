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

	virtual const QFont& textFont(const QModelIndex& index) const noexcept override;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QPixmap& decoration(const QModelIndex& index) const noexcept override;
	virtual int textAlignmentFlags(const QModelIndex& index) const noexcept override;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;
	virtual IGridViewResizePolicy* resizeStrategy() const noexcept override;

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