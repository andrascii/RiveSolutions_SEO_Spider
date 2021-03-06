#pragma once

namespace SeoSpider
{

class IRenderer;
class IResizePolicy;

class IViewModel
{
public:
	virtual ~IViewModel() = default;

	virtual int marginTop(const QModelIndex& index) const noexcept = 0;
	virtual int marginBottom(const QModelIndex& index) const noexcept = 0;
	virtual int marginLeft(const QModelIndex& index) const noexcept = 0;
	virtual int marginRight(const QModelIndex& index) const noexcept = 0;

	virtual QPixmap pixmap(const QModelIndex& index) const noexcept = 0;
	virtual QRect pixmapPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept = 0;

	virtual QString displayData(const QModelIndex& index, const QRect& itemVisualRect) const noexcept = 0;
	virtual QRect displayDataPosition(const QModelIndex& index, const QRect& itemVisualRect) const noexcept = 0;

	virtual const QColor& selectedBackgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& hoveredBackgroundColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& backgroundColor(const QModelIndex& index) const noexcept = 0;

	virtual const QFont& font(const QModelIndex& index) const noexcept = 0;
	virtual int textAlignment(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& textColor(const QModelIndex& index) const noexcept = 0;

	virtual const QColor& selectedGridLineColor(const QModelIndex& index) const noexcept = 0;
	virtual const QColor& gridLineColor(const QModelIndex& index) const noexcept = 0;

	virtual void invalidateItemViewRendererCache() const noexcept = 0;
	virtual const IRenderer* itemViewRenderer(const QModelIndex& index) const noexcept = 0;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept = 0;
	virtual const QModelIndex& hoveredIndex() const noexcept = 0;

	virtual void setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept = 0;
	virtual void setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept = 0;
	virtual const QModelIndexList& selectedIndexes() const noexcept = 0;

    virtual double devicePixelRatio() const = 0;

	virtual QObject* qobject() noexcept = 0;

	// signal
	virtual void repaintItems(const QModelIndexList& modelIndexes) const = 0;
};

}
