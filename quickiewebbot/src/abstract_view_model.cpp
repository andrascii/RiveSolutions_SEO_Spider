#include "abstract_view_model.h"
#include "irenderer.h"

namespace QuickieWebBot
{

AbstractViewModel::AbstractViewModel(AbstractTableModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_hoveredIndex(QModelIndex())
	, m_previousHoveredIndex(QModelIndex())
	, m_itemRenderer(this)
{
}

const QModelIndex& AbstractViewModel::hoveredIndex() const noexcept
{
	return m_hoveredIndex;
}

void AbstractViewModel::setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			continue;
		}

		m_selectedModelIndexes.append(modelIndexes[i]);
	}

	emitNeedToRepaintIndexes(m_selectedModelIndexes);
}

void AbstractViewModel::setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			m_selectedModelIndexes.removeOne(modelIndexes[i]);
		}
	}
	
// 	const int row = modelIndexes.at(0);
// 
// 	QModelIndexList repaintModelIndexes = modelIndexes;
// 	repaintModelIndexes.append(model()->modelIndexesForRow())

	emitNeedToRepaintIndexes(modelIndexes);
}

const QModelIndexList& AbstractViewModel::selectedIndexes() const noexcept
{
	return m_selectedModelIndexes;
}

void AbstractViewModel::invalidateItemViewRendererCache() const noexcept
{
	m_itemRenderer.invalidateCache();
}

const IRenderer* AbstractViewModel::itemViewRenderer(const QModelIndex& index) const noexcept
{
	return &m_itemRenderer;
}

void AbstractViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	if (index == m_hoveredIndex)
	{
		return;
	}

	const QModelIndex previousHoveredIndex = m_hoveredIndex;

	m_hoveredIndex = index;

	setPreviousHoveredIndex(previousHoveredIndex);
}

QObject* AbstractViewModel::qobject() noexcept
{
	return this;
}

const QuickieWebBot::AbstractTableModel* AbstractViewModel::model() const noexcept
{
	return m_model;
}

AbstractTableModel* AbstractViewModel::model() noexcept
{
	return m_model;
}

const QModelIndex& AbstractViewModel::previousHoveredIndex() const noexcept
{
	return m_previousHoveredIndex;
}

const void AbstractViewModel::resetPreviousHoveredIndex() noexcept
{
	m_previousHoveredIndex = QModelIndex();
}

void AbstractViewModel::emitNeedToRepaintIndexes(const QModelIndexList& modelIndexes) noexcept
{
	invalidateCacheIndexes(modelIndexes);

	Q_EMIT repaintItems(modelIndexes);
}

void AbstractViewModel::addRenderer(int rendererTypes)
{
	m_itemRenderer.addRenderer(rendererTypes);
}

void AbstractViewModel::setItemRendererCacheSize(int cacheSize)
{
	m_itemRenderer.setCacheSize(cacheSize);
}

void AbstractViewModel::invalidateCacheIndexes(const QModelIndexList& modelIndexes) noexcept
{
	foreach(const QModelIndex& index, modelIndexes)
	{
		m_itemRenderer.invalidateCacheIndex(index);
	}
}

void AbstractViewModel::invalidateCacheIndex(const QModelIndex& index) noexcept
{
	m_itemRenderer.invalidateCacheIndex(index);
}

void AbstractViewModel::clearSelectedIndexes() noexcept
{
	m_selectedModelIndexes.clear();
}

void AbstractViewModel::setPreviousHoveredIndex(const QModelIndex& index)
{
	m_previousHoveredIndex = index;
}

}