#include "table_view.h"
#include "delegate.h"
#include "irenderer.h"
#include "itable_model.h"
#include "iview_model.h"
#include "text_renderer.h"
#include "background_renderer.h"

namespace QuickieWebBot
{

Delegate::Delegate(TableView* parent)
	: QStyledItemDelegate(parent)
	, m_view(parent)
{
}

void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!m_view->viewModel())
	{
		ERRLOG << "For this view did not installed ViewModel!";
		return;
	}

	foreach(const IRenderer* renderer, m_view->viewModel()->renderers(index))
	{
		renderer->render(painter, option, index);
	}
}

bool Delegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return false;
}

}