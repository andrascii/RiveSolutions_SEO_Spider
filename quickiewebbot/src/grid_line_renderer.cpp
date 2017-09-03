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
	const QPoint secondPointBottomLine(adjustedRect.x() + adjustedRect.width(), adjustedRect.y() + adjustedRect.height());

	QColor bottomLineColor = m_viewModel->hoveredBackgroundColor(index);

	const QPoint firstPointRightVerticalLine(adjustedRect.x() + adjustedRect.width(), adjustedRect.y() + adjustedRect.height());
	const QPoint secondPointRightVerticalLine(adjustedRect.x() + adjustedRect.width(), adjustedRect.y());

	QPen oldPen = painter->pen();

	painter->setPen(bottomLineColor);
	painter->drawLine(firstPointBottomLine, secondPointBottomLine);

	painter->setPen(oldPen);
	painter->drawLine(firstPointRightVerticalLine, secondPointRightVerticalLine);
}

void GridLineRenderer::renderHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept
{
	const bool isThisItemRowHovered = m_viewModel->hoveredIndex().row() == index.row();

	QColor rectangleColor = isThisItemRowHovered ?
		m_viewModel->hoveredBackgroundColor(index) : m_gridLineColor;

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
	
	const bool isThisItemRowAboveHovered = m_viewModel->hoveredIndex().row() + 1 == index.row();

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

}