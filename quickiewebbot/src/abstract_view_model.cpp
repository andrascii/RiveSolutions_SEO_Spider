#include "abstract_view_model.h"
#include "irenderer.h"

namespace QuickieWebBot
{

AbstractViewModel::AbstractViewModel(AbstractTableModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_hoveredIndex(QModelIndex())
	, m_previousHoveredIndex(QModelIndex())
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
		if (!m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			m_selectedModelIndexes.append(modelIndexes[i]);
		}
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

	emitNeedToRepaintIndexes(m_selectedModelIndexes);
}

const QModelIndexList& AbstractViewModel::selectedIndexes() const noexcept
{
	return m_selectedModelIndexes;
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

QList<const IRenderer*> AbstractViewModel::renderers(const QModelIndex& index) const noexcept
{
	QList<const IRenderer*> renderers;

	for(const auto& p : m_renderers)
	{
		renderers.append(p.second.get());
	}

	return renderers;
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

void AbstractViewModel::addRenderer(RendererType rendererType, IRenderer* renderer)
{
	DEBUG_ASSERT(m_renderers.find(rendererType) == m_renderers.end());

	m_renderers[rendererType] = std::unique_ptr<IRenderer>(renderer);
}

IRenderer* AbstractViewModel::renderer(RendererType rendererType) const noexcept
{
	auto rendererIterator = m_renderers.find(rendererType);

	return rendererIterator == m_renderers.end() ? nullptr : rendererIterator->second.get();
}

const QModelIndex& AbstractViewModel::previousHoveredIndex() const noexcept
{
	return m_previousHoveredIndex;
}

void AbstractViewModel::emitNeedToRepaintIndexes(const QModelIndexList& modelIndexes) noexcept
{
	invalidateCacheIndexes(modelIndexes);

	Q_EMIT repaintItems(modelIndexes);
}

void AbstractViewModel::invalidateCacheIndexes(const QModelIndexList& indexesList)
{
	foreach(const QModelIndex& index, indexesList)
	{
		invalidateCacheIndex(index);
	}
}

void AbstractViewModel::invalidateCacheIndex(const QModelIndex& index)
{
	for (const auto& pair : m_renderers)
	{
		pair.second->invalidateCacheIndex(index);
	}
}

void AbstractViewModel::setPreviousHoveredIndex(const QModelIndex& index)
{
	m_previousHoveredIndex = index;
}

void AbstractViewModel::invalidateRenderersCache() const noexcept
{
	for (const auto& pair : m_renderers)
	{
		pair.second->invalidateCache();
	}
}

}