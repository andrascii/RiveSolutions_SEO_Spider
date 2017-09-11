#pragma once

#include "abstract_view_model.h"
#include "item_renderer.h"

namespace QuickieWebBot
{

class SummaryModel;
class ViewportPercentResizePolicy;

class SummaryViewModel : public AbstractViewModel
{
	Q_OBJECT

public:
	SummaryViewModel(SummaryModel* model, QObject* parent = nullptr);

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept override;
	virtual QRect pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;

	virtual QString displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;
	virtual QRect displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept override;

	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;

	virtual const QFont& font(const QModelIndex& index) const noexcept override;
	virtual Qt::AlignmentFlag textAlignment(const QModelIndex& index) const noexcept override;
	virtual QColor textColor(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;

private:
	void initializeRenderers();

private:
	QColor m_selectionBgColor;
	QColor m_hoveredBgColor;
	QColor m_bgColor;

	ItemRenderer m_itemRenderer;
};

}
