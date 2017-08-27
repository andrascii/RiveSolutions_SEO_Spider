#include "background_renderer.h"
#include "iview_model.h"

namespace QuickieWebBot
{

BackgroundRenderer::BackgroundRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void BackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QColor bgColor;

	if (m_viewModel->hoveredIndex().row() == index.row())
	{
		bgColor = QColor("grey");
	}
	else
	{
		bgColor = qvariant_cast<QColor>(index.data(Qt::BackgroundColorRole));
	}

	painter->fillRect(option.rect, bgColor);
}

void BackgroundRenderer::resetCache()
{
}

void BackgroundRenderer::setCacheSize(int)
{
}

}