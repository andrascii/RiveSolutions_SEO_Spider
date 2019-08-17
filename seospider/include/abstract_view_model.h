#pragma once

#include "iview_model.h"
#include "abstract_table_model.h"
#include "item_renderer.h"

namespace SeoSpider
{

class IRenderer;

class AbstractViewModel : public QObject, public IViewModel
{
	Q_OBJECT

public:
	AbstractViewModel(AbstractTableModel* model, float devicePixelRatio, QObject* parent = nullptr);

	virtual const QModelIndex& hoveredIndex() const noexcept override;
	virtual void setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual void setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual const QModelIndexList& selectedIndexes() const noexcept override;
	virtual void invalidateItemViewRendererCache() const noexcept override;
	virtual const IRenderer* itemViewRenderer(const QModelIndex& index) const noexcept override;
	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;
	virtual float devicePixelRatio() const override;
	virtual QObject* qobject() noexcept override;

signals:
	virtual void repaintItems(const QModelIndexList& modelIndexes) const override;

protected:
	const AbstractTableModel* model() const noexcept;
	AbstractTableModel* model() noexcept;
	const QModelIndex& previousHoveredIndex() const noexcept;
	void resetPreviousHoveredIndex() noexcept;
	void clearSelectedIndexes() noexcept;
	void emitNeedToRepaintIndexes(const QModelIndexList& modelIndexes) noexcept;
	void addRenderer(int rendererTypes);
	void setItemRendererCacheSize(int cacheSize);
	void invalidateCacheIndexes(const QModelIndexList& modelIndexes) const noexcept;
	void invalidateCacheIndex(const QModelIndex& index) const noexcept;

	Q_SLOT void onAttachedModelDataChanged(const QModelIndex& startIndex, const QModelIndex& endIndex, const QVector<int>& roles);

private:
	void setPreviousHoveredIndex(const QModelIndex& index);

protected:
	QModelIndex m_previousHoveredIndex;
	QModelIndex m_hoveredUnderlyingIndex;
	QModelIndex m_hoveredIndex;;

private:
	AbstractTableModel* m_model;
	QModelIndexList m_selectedModelIndexes;
	ItemRenderer m_itemRenderer;
	float m_devicePixelRatio;
};

}