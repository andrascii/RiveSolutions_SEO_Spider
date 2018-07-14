#include "abstract_view_model.h"
#include "irenderer.h"
#include "model_helpers.h"

namespace SeoSpider
{

AbstractViewModel::AbstractViewModel(AbstractTableModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_previousHoveredIndex(QModelIndex())
	, m_itemRenderer(this)
{
}

const QModelIndex& AbstractViewModel::hoveredIndex() const noexcept
{
	return m_hoveredUnderlyingIndex;
}

void AbstractViewModel::setSelectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	const auto topSelectedRow = [this]() -> QModelIndex
	{
		QModelIndex result;

		foreach(const QModelIndex& index, selectedIndexes())
		{
			if (!result.isValid() || index.row() < result.row())
			{
				result = index;
			}
		}

		return result;
	};

	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			continue;
		}

		m_selectedModelIndexes.append(modelIndexes[i]);
	}

	const QModelIndex topSelectedRowIndex = topSelectedRow();

	if (topSelectedRowIndex.isValid() && model()->rowCount() > topSelectedRowIndex.row() + 1)
	{
		emitNeedToRepaintIndexes(makeRowIndexes(topSelectedRowIndex.model()->index(topSelectedRowIndex.row() + 1, topSelectedRowIndex.column())));
	}

	emitNeedToRepaintIndexes(m_selectedModelIndexes);
}

void AbstractViewModel::setDeselectedIndexes(const QModelIndexList& modelIndexes) noexcept
{
	const auto rowsForRepaintHelper = [this, &modelIndexes]() -> QVector<QModelIndex>
	{
		if (!m_selectedModelIndexes.size())
		{
			return QVector<QModelIndex>();
		}

		QVector<QModelIndex> uniqueSelectedRows;

		foreach(const QModelIndex& index, m_selectedModelIndexes)
		{
			uniqueSelectedRows.push_back(index);
		}

		std::sort(std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows));
		uniqueSelectedRows.erase(std::unique(std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows)), std::end(uniqueSelectedRows));

		QVector<QModelIndex>::iterator topRowElement = std::min_element(
			std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows), [](const QModelIndex& index1, const QModelIndex& index2)
		{
			return index1.row() < index2.row();
		});

		QVector<QModelIndex>::iterator footRowElement = std::max_element(
			std::begin(uniqueSelectedRows), std::end(uniqueSelectedRows), [](const QModelIndex& index1, const QModelIndex& index2)
		{
			return index1.row() > index2.row();
		});

		const QModelIndex topRow = *topRowElement;
		const QModelIndex footRow = *footRowElement;

		QVector<QModelIndex> rowsForRepaint;

		for (int i = topRow.row(); i <= footRow.row(); ++i)
		{
			QModelIndex rowIndex = topRow.model()->index(i, 0);
			QModelIndex nextRowIndex = topRow.model()->index(i + 1, 0);
			const bool isNonSelectedRowBelowSelectedRow = 
				!uniqueSelectedRows.contains(nextRowIndex) && uniqueSelectedRows.contains(rowIndex);

			if (!isNonSelectedRowBelowSelectedRow)
			{
				continue;
			}

			rowsForRepaint.push_back(nextRowIndex);
		}

		return rowsForRepaint;
	};

	const QVector<QModelIndex> rowsForRepaint = rowsForRepaintHelper();

	for (int i = 0; i < modelIndexes.size(); ++i)
	{
		if (m_selectedModelIndexes.contains(modelIndexes[i]))
		{
			m_selectedModelIndexes.removeOne(modelIndexes[i]);
		}
	}

	if (rowsForRepaint.size())
	{
		QModelIndexList itemsForRepaintAfterDeselect;

		foreach(const QModelIndex& row, rowsForRepaint)
		{
			if (row.row() < model()->rowCount())
			{
				itemsForRepaintAfterDeselect.append(makeRowIndexes(row));
			}
		} 

		emitNeedToRepaintIndexes(itemsForRepaintAfterDeselect);
	}

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

const IRenderer* AbstractViewModel::itemViewRenderer(const QModelIndex&) const noexcept
{
	return &m_itemRenderer;
}

void AbstractViewModel::setHoveredIndex(const QModelIndex& index) noexcept
{
	QModelIndex underlyingIndex = getUnderlyingIndex(index);
	if (underlyingIndex == m_hoveredUnderlyingIndex)
	{
		return;
	}

	const QModelIndex previousHoveredIndex = m_hoveredUnderlyingIndex;
	m_hoveredUnderlyingIndex = underlyingIndex;
	m_hoveredIndex = index;

	setPreviousHoveredIndex(previousHoveredIndex);
}

QObject* AbstractViewModel::qobject() noexcept
{
	return this;
}

const SeoSpider::AbstractTableModel* AbstractViewModel::model() const noexcept
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

void AbstractViewModel::resetPreviousHoveredIndex() noexcept
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

void AbstractViewModel::invalidateCacheIndexes(const QModelIndexList& modelIndexes) const noexcept
{
	foreach(const QModelIndex& index, modelIndexes)
	{
		m_itemRenderer.invalidateCacheIndex(index);
	}
}

void AbstractViewModel::invalidateCacheIndex(const QModelIndex& index) const noexcept
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