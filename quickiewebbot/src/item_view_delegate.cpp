#include "table_view.h"
#include "item_view_delegate.h"
#include "irenderer.h"
#include "abstract_table_model.h"
#include "iview_model.h"
#include "text_renderer.h"
#include "background_renderer.h"

namespace QuickieWebBot
{

ItemViewDelegate::ItemViewDelegate(IViewModel* viewModel, QObject* parent)
	: QStyledItemDelegate(parent)
	, m_viewModel(viewModel)
{
}

void ItemViewDelegate::setViewModel(IViewModel* viewModel) noexcept
{
	m_viewModel = viewModel;
}

IViewModel* ItemViewDelegate::viewModel() const noexcept
{
	return m_viewModel;
}

void ItemViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!m_viewModel)
	{
		ERRORLOG << "For this view did not installed ViewModel!";
		return;
	}

	foreach(const IRenderer* renderer, m_viewModel->renderers(index))
	{
		renderer->render(painter, option, index);
	}
}

bool ItemViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return false;
}

}