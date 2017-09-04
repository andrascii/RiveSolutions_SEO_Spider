#pragma once

#include "iview_model.h"
#include "abstract_table_model.h"

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

	virtual void setHoveredIndex(const QModelIndex& index) noexcept override;

	virtual void invalidateRenderersCache() const noexcept override;
	virtual QList<const IRenderer*> renderers(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void repaintItems(const QModelIndexList& modelIndexes) const override;

protected:
	enum RendererType
	{
		PlainTextRendererType,
		UrlRendererType,
		BackgroundRendererType,
		SelectionBackgroundRendererType,
		GridLineRendererType
	};

	const AbstractTableModel* model() const noexcept;
	AbstractTableModel* model() noexcept;

	void addRenderer(RendererType rendererType, IRenderer* renderer);
	IRenderer* renderer(RendererType rendererType) const noexcept;

	const QModelIndex& previousHoveredIndex() const noexcept;

	void emitNeedToRepaintIndexes(const QModelIndexList& modelIndexes) noexcept;

private:
	void invalidateCacheIndexes(const QModelIndexList& indexesList);
	void invalidateCacheIndex(const QModelIndex& index);

	void setPreviousHoveredIndex(const QModelIndex& index);

private:
	AbstractTableModel* m_model;

	QModelIndex m_hoveredIndex;
	QModelIndex m_previousHoveredIndex;

	QModelIndexList m_selectedModelIndexes;

	std::map<RendererType, std::unique_ptr<IRenderer>> m_renderers;
};

}
