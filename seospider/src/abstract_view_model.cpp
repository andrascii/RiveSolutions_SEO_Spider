#include "stdafx.h"
#include "abstract_view_model.h"
#include "irenderer.h"
#include "model_helpers.h"

namespace SeoSpider
{

AbstractViewModel::AbstractViewModel(AbstractTableModel* model, float devicePixelRatio, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_previousHoveredIndex(QModelIndex())
	, m_itemRenderer(this)
	, m_devicePixelRatio(devicePixelRatio)
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

	QModelIndexList itemsForRepaintAfterDeselect;
	const int rowCount = model()->rowCount();
	if (rowsForRepaint.size())
	{
		foreach(const QModelIndex& row, rowsForRepaint)
		{
			if (row.row() < rowCount)
			{
				itemsForRepaintAfterDeselect.append(makeRowIndexes(row));
			}
		}
	}

	foreach(const QModelIndex& index, modelIndexes)
	{
		if (index.row() < rowCount)
		{
			itemsForRepaintAfterDeselect.append(index);
		}
	}

	emitNeedToRepaintIndexes(itemsForRepaintAfterDeselect);
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

float AbstractViewModel::devicePixelRatio() const
{
    return m_devicePixelRatio;
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

void AbstractViewModel::onAttachedModelDataChanged(const QModelIndex& startIndex, const QModelIndex& endIndex, const QVector<int>& roles)
{
	Q_UNUSED(roles);
	DEBUG_ASSERT(startIndex.isValid() && endIndex.isValid());
	// TODO: is's more intelligent to check [rows in cache/visible rows]
	// and check if these indices are axist in the cache before remove the corresponding rows from cache
	// and before adding them to 'indices' (because we can add to indices a lot of values)
	// but probably it'll be better just to clear the cache instead of this
	constexpr int maxRowsToUpdate = 70;
	const int invalidateAllCache = qAbs(startIndex.row() - endIndex.row()) > maxRowsToUpdate;
	if (invalidateAllCache)
	{
		// optimization
		m_itemRenderer.invalidateCache();
	}
	else
	{
		QModelIndexList indices;

		const int columns = startIndex.model()->columnCount();
		for (int row = startIndex.row(); row <= endIndex.row(); ++row)
		{
			// TODO: optimize
			for (int column = 0; column < columns; ++column)
			{
				indices.append(startIndex.model()->index(row, column));
			}
		}

		DEBUG_ASSERT(indices.size() > 0);

		invalidateCacheIndexes(indices);
	}

	if (!invalidateAllCache)
	{
		invalidateCacheIndexes(makeRowIndexes(m_hoveredUnderlyingIndex));
	}
	emit repaintItems(makeRowIndexes(m_hoveredUnderlyingIndex));

	m_hoveredUnderlyingIndex = getUnderlyingIndex(m_hoveredIndex);
	if (!invalidateAllCache)
	{
		invalidateCacheIndexes(makeRowIndexes(m_hoveredUnderlyingIndex));
	}
	emit repaintItems(makeRowIndexes(m_hoveredUnderlyingIndex));
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