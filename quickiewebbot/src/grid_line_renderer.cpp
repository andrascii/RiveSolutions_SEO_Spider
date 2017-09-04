#include "grid_line_renderer.h"

namespace QuickieWebBot
{

GridLineRenderer::GridLineRenderer(const IViewModel* viewModel, QColor gridLineColor)
	: m_viewModel(viewModel)
	, m_gridLineColor(gridLineColor)
{
}

void GridLineRenderer::setCacheSize(int)
{
}

void GridLineRenderer::renderThisIfItAboveHoveredHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept
{
	const QPoint firstPointBottomLine(adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	const QPoint secondPointBottomLine(adjustedRect.width() + adjustedRect.x(), adjustedRect.y() + adjustedRect.height());

	const bool isThisRowSelected = isRowSelected(index.row());

	QColor bottomLineColor = isThisRowSelected ?
		m_viewModel->selectionBackgroundColor(index) : 
		m_viewModel->hoveredBackgroundColor(index);

	QColor rightVerticalLineColor = isThisRowSelected ? m_viewModel->selectionBackgroundColor(index) : m_gridLineColor;

	const QPoint firstPointRightVerticalLine(adjustedRect.x() + adjustedRect.width(), adjustedRect.y() + adjustedRect.height());
	const QPoint secondPointRightVerticalLine(adjustedRect.x() + adjustedRect.width(), adjustedRect.y());

	painter->save();
	painter->setPen(bottomLineColor);
	painter->drawLine(firstPointBottomLine, secondPointBottomLine);

	painter->setPen(rightVerticalLineColor);
	painter->drawLine(firstPointRightVerticalLine, secondPointRightVerticalLine);
	painter->restore();
}

void GridLineRenderer::renderHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept
{
	QColor rectangleColor = m_gridLineColor;
	
	if (isRowSelected(index.row()))
	{
		rectangleColor = m_viewModel->selectionBackgroundColor(index);
	}
	else if (m_viewModel->hoveredIndex().row() == index.row())
	{
		rectangleColor = m_viewModel->hoveredBackgroundColor(index);
	}
	else
	{
		rectangleColor = m_gridLineColor;
	}

	painter->save();
	painter->setPen(rectangleColor);

	QPainterPath path;

	path.moveTo(adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	path.lineTo(adjustedRect.width() + adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	path.lineTo(adjustedRect.width() + adjustedRect.x(), adjustedRect.y());

	painter->drawPath(path);

	painter->restore();
}

void GridLineRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QRect rect = option.rect.adjusted(0, 0, -1, -1);

	const bool isThisItemRowAboveHovered = m_viewModel->hoveredIndex().row() - 1 == index.row() || isRowSelected(index.row() + 1);

	if (isThisItemRowAboveHovered)
	{
		renderThisIfItAboveHoveredHelper(painter, rect, index);
		return;
	}

	renderHelper(painter, rect, index);
}

void GridLineRenderer::invalidateCacheIndex(const QModelIndex&) const
{
}

void GridLineRenderer::invalidateCache() const
{
}

bool GridLineRenderer::isRowSelected(int row) const noexcept
{
	foreach(const QModelIndex& index, m_viewModel->selectedIndexes())
	{
		if (index.row() == row)
		{
			return true;
		}
	}

	return false;
}

}