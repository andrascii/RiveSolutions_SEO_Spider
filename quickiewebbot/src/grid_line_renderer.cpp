#include "grid_line_renderer.h"

namespace QuickieWebBot
{

GridLineRenderer::GridLineRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void GridLineRenderer::setCacheSize(int)
{
}

void GridLineRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
}

void GridLineRenderer::renderThisIfItAboveHoveredHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept
{
	const QPoint firstPointBottomLine(adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	const QPoint secondPointBottomLine(adjustedRect.width() + adjustedRect.x(), adjustedRect.y() + adjustedRect.height());

	const bool isThisRowSelected = isRowSelected(index.row());
	const bool isBelowRowSelected = isRowSelected(index.row() + 1);

	const QColor bottomLineColor = isThisRowSelected || isBelowRowSelected ?
		m_viewModel->selectedGridLineColor(index) : 
		m_viewModel->gridLineColor(index);

	const QColor rightVerticalLineColor = isThisRowSelected ?
		m_viewModel->selectedGridLineColor(index) :
		m_viewModel->gridLineColor(index);

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
	const QColor rectangleColor = isRowSelected(index.row()) ?
		m_viewModel->selectedGridLineColor(index) : 
		m_viewModel->gridLineColor(index);

	painter->save();
	painter->setPen(rectangleColor);

	QPainterPath path;

	path.moveTo(adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	path.lineTo(adjustedRect.width() + adjustedRect.x(), adjustedRect.y() + adjustedRect.height());
	path.lineTo(adjustedRect.width() + adjustedRect.x(), adjustedRect.y());

	painter->drawPath(path);

	painter->restore();
}

void GridLineRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QRect rect = option.rect.adjusted(0, 0, -1, -1);

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