#include "grid_view.h"
#include "grid_view_delegate.h"
#include "irenderer.h"
#include "igrid_model.h"
#include "igrid_view_model.h"
#include "text_renderer.h"
#include "background_renderer.h"

namespace QuickieWebBot
{

GridViewDelegate::GridViewDelegate(GridView* parent)
	: QStyledItemDelegate(parent)
	, m_gridView(parent)
{
	// TODO: install event filter
}

void GridViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!m_gridView->viewModel())
	{
		ERRORLOG << "For this view did not installed ViewModel!";
		return;
	}

	foreach(const IRenderer* gridViewPainter, m_gridView->viewModel()->renderers(index))
	{
		gridViewPainter->render(painter, option, index);
	}
}

bool GridViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return false;
}

}