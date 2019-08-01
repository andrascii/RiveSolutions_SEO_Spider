#include "stdafx.h"
﻿#include "header_view.h"
#include "model_helpers.h"
#include "svg_renderer.h"
#include "deferred_call.h"

namespace
{

// this menu is not disapears when its action is clicked
// the implementations is really not good
// TODO: make a special class inherited from QFrame fot this menu
class CustomMenu : public QMenu
{
public:
	CustomMenu(QWidget* parent)
		: QMenu(parent)
		, m_canHide(true)
	{
	}

	virtual void setVisible(bool visible) override
	{
		if (!visible && !m_canHide)
		{
			return;
		}

		QMenu::setVisible(visible);
	}

	virtual void mousePressEvent(QMouseEvent* event) override
	{
		if (rect().contains(event->localPos().toPoint()))
		{
			m_canHide = false;
		}
		QMenu::mousePressEvent(event);
	}

	virtual void mouseReleaseEvent(QMouseEvent* event) override
	{
		SeoSpider::DeferredCall::call(this, [this]()
		{
			m_canHide = true;
		});
		QMenu::mouseReleaseEvent(event);
	}

	virtual void focusOutEvent(QFocusEvent* event) override
	{
		m_canHide = true;
		QMenu::focusOutEvent(event);
	}

private:
	bool m_canHide;
};


QIcon& gearIcon()
{
	static QIcon s_gearIcon = SeoSpider::SvgRenderer::render(QStringLiteral(":/images/gear.svg"), 7, 7);
	return s_gearIcon;
}
}

namespace SeoSpider
{
HeaderView::HeaderView(QWidget* parent, bool showCustomizeColumnsButton)
	: QHeaderView(Qt::Horizontal, parent)
	, m_showCustomizeColumnsButton(showCustomizeColumnsButton)
	, m_button(nullptr)
	, m_tableView(qobject_cast<QTableView*>(parent))
	, m_menu(nullptr)
{
	setSectionsClickable(true);
	// setFirstSectionMovable(false); // available from 5.11

	if (showCustomizeColumnsButton)
	{
		m_button = new QPushButton("", this);
		m_button->setObjectName("customizeColumns");
		m_button->setIcon(gearIcon());
		m_menu = new CustomMenu(m_button);
		m_button->setMenu(m_menu);
	}
}

void HeaderView::setModel(QAbstractItemModel* model)
{
	QHeaderView::setModel(model);

	updateActions();

	// we need a signal like QHeaderView::sectionMoved, but there is no such signal
	// QAbstractItemModel::columnsMoved is not working because we don't really move columns in the model
	// VERIFY(connect(model, &QAbstractItemModel::columnsMoved, this, &HeaderView::updateMenu));
}

void HeaderView::paintEvent(QPaintEvent * event)
{
	QHeaderView::paintEvent(event);

	if (!m_showCustomizeColumnsButton)
	{
		return;
	}

	QPainter painter(viewport());

	QRect rect(0, 0, sectionSize(logicalIndex(0)) - 1, height());

	paintSection(&painter, rect.adjusted(m_button->width(), 0, 1, 0), logicalIndex(0));
}

void HeaderView::resizeEvent(QResizeEvent* event)
{
	QHeaderView::resizeEvent(event);
	if (m_showCustomizeColumnsButton)
	{
		m_button->move(0, 1);
	}
}

void HeaderView::enterEvent(QEvent* event)
{
	QHeaderView::enterEvent(event);
	if (m_showCustomizeColumnsButton)
	{
		// m_button->show();
	}
}

void HeaderView::leaveEvent(QEvent* event)
{
	QHeaderView::leaveEvent(event);
	if (m_showCustomizeColumnsButton)
	{
		// m_button->hide();
	}
}

void HeaderView::initColumns()
{
	for (int i = 0; i < count(); ++i)
	{
		const int lIndex = logicalIndex(i);
		const bool columnEnabled = model()->headerData(lIndex, Qt::Horizontal, AbstractTableModel::columnEnabledRole).toBool();

		if (!columnEnabled && !isSectionHidden(lIndex))
		{
			m_sectionSize[lIndex] = sectionSize(lIndex);
			hideSection(lIndex);
		}
	}
}

void HeaderView::updateActions()
{
	if (!m_showCustomizeColumnsButton)
	{
		return;
	}

	const QAbstractItemModel* underlyingModel = getUnderlyingModel<const QAbstractItemModel*>(model());

	if (underlyingModel == nullptr || count() == 0)
	{
		return;
	}

	for (int i = 0; i < count(); ++i)
	{
		const int lIndex = logicalIndex(i);
		if (lIndex == 0)
		{
			// # column
			// TODO: check it more clever: something like
			// model()->headerData(lIndex, Qt::Horizontal, AbstractTableModel::columnAlwaysEnabledRole)
			continue;
		}

		const QString text = underlyingModel->headerData(lIndex, Qt::Horizontal).toString();
		QAction* action = new QAction(text, this);
		action->setCheckable(true);
		const bool columnEnabled = model()->headerData(lIndex, Qt::Horizontal, AbstractTableModel::columnEnabledRole).toBool();
		action->setChecked(columnEnabled);
		action->setData(lIndex);
		m_actions[lIndex] = action;
		m_menu->addAction(action);

		VERIFY(connect(action, &QAction::toggled, this, &HeaderView::onActionToggled));
	}
}

void HeaderView::updateMenu()
{
	if (!m_showCustomizeColumnsButton)
	{
		return;
	}

	m_menu->clear();
	for (int i = 0; i < count(); ++i)
	{
		const int lIndex = logicalIndex(i);
		if (lIndex == 0)
		{
			// # column
			// TODO: check it more clever: something like
			// model()->headerData(lIndex, Qt::Horizontal, AbstractTableModel::columnAlwaysEnabledRole)
			continue;
		}

		QAction* action = m_actions[lIndex];
		m_menu->addAction(action);
	}
}

void HeaderView::onActionToggled(bool enabled)
{
	QAction* action = qobject_cast<QAction*>(sender());
	ASSERT(action);
	const int column = action->data().toInt();
	if (!enabled)
	{
		m_sectionSize[column] = sectionSize(column);
	}

	setSectionHidden(column, !enabled);

	if (enabled)
	{
		resizeSection(column, m_sectionSize[column]);
	}

	// TODO: update the model and the corresponding storage adapter
}

}
