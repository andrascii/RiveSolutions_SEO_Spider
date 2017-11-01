#pragma once

#include "abstract_view_model.h"
#include "item_renderer.h"

namespace SeoSpider
{

class PageModel;

class PageViewModel : public AbstractViewModel
{
	Q_OBJECT

public:
	PageViewModel(PageModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
	virtual QRect pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;

	virtual QString displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;
	virtual QRect displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;

	virtual const QColor& selectedBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;

	virtual const QFont& font(const QModelIndex& index) const noexcept override;
	virtual int textAlignment(const QModelIndex& index) const noexcept override;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept override;

	virtual const QColor& selectedGridLineColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& gridLineColor(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;

private:
	Q_SLOT void onAttachedModelInternalDataChanged();

private:
	void initializeRenderers();

private:
	QColor m_selectionBackgroundColor;
	QColor m_hoveredBackgroundColor;
	QColor m_backgroundColor;
	QColor m_selectedGridLineColor;
	QColor m_gridLineColor;

	QColor m_urlTextColor;
	QColor m_textColor;

	QFont m_textFont;

	QPixmap m_urlIcon;

	ItemRenderer m_itemRenderer;
};

}