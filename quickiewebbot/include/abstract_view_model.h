#pragma once

#include "iview_model.h"
#include "abstract_table_model.h"
#include "item_renderer.h"

namespace QuickieWebBot
{

class IRenderer;

class AbstractViewModel : public QObject, public IViewModel
{
	Q_OBJECT

public:
	AbstractViewModel(AbstractTableModel* model, QObject* parent = nullptr);

	virtual const QModelIndex& hoveredIndex() const noexcept override;

	virtual void setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual void setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept override;
	virtual const QModelIndexList& selectedIndexes() const noexcept override;

	virtual void invalidateItemViewRendererCache() const noexcept override;
	virtual const IRenderer* itemViewRenderer(const QModelIndex& index) const noexcept override;

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;
	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void repaintItems(const QModelIndexList& modelIndexes) const override;

protected:
	const AbstractTableModel* model() const noexcept;
	AbstractTableModel* model() noexcept;

	const QModelIndex& previousHoveredIndex() const noexcept;
	const void resetPreviousHoveredIndex() noexcept;
	void clearSelectedIndexes() noexcept;

	void emitNeedToRepaintIndexes(const QModelIndexList& modelIndexes) noexcept;

	void addRenderer(int rendererTypes);
	void setItemRendererCacheSize(int cacheSize);
	void invalidateCacheIndexes(const QModelIndexList& modelIndexes) noexcept;
	void invalidateCacheIndex(const QModelIndex& index) noexcept;

private:
	void setPreviousHoveredIndex(const QModelIndex& index);

private:
	AbstractTableModel* m_model;

	QModelIndex m_hoveredIndex;
	QModelIndex m_previousHoveredIndex;

	QModelIndexList m_selectedModelIndexes;

	ItemRenderer m_itemRenderer;
};

}
