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

	virtual int marginTop(const QModelIndex& index) const noexcept override;
	virtual int marginBottom(const QModelIndex& index) const noexcept override;
	virtual int marginLeft(const QModelIndex& index) const noexcept override;
	virtual int marginRight(const QModelIndex& index) const noexcept override;

	virtual const QPixmap& itemPixmap(const QModelIndex& index) const noexcept override;
	virtual QRect itemPixmapPosition(const QModelIndex& index) const noexcept override;

	virtual const QColor& selectionBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept override;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept override;

	virtual const QFont& font(const QModelIndex& index) const noexcept override;
	virtual Qt::AlignmentFlag textAlignment(const QModelIndex& index) const noexcept override;
	virtual QColor textColor(const QModelIndex& index) const noexcept override;

	virtual void invalidateRenderersCache() const noexcept override;
	virtual QList<const IRenderer*> renderers(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;
	virtual const QModelIndex& hoveredIndex() const noexcept override;
	
	virtual QObject* qobject() noexcept override;

private:
	void invalidateCacheIndex(const QModelIndex& index);

private:
	SummaryModel* m_model;

	std::unique_ptr<ViewportPercentResizePolicy> m_resizePolicy;

	QModelIndex m_hoveredIndex;

	QColor m_selectionBgColor;
	QColor m_hoveredBgColor;
	QColor m_bgColor;
};

}
