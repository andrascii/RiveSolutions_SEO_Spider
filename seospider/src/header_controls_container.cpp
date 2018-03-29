#include "header_controls_container.h"
#include "helpers.h"

namespace SeoSpider
{

QWidget* HeaderToolButtonCreator::createControl(QAction* action)
{
	QToolButton* button = new QToolButton;

	auto updateControl = [action, button]()
	{
		button->setToolTip(action->text());
		button->setIcon(action->icon());
		button->setEnabled(action->isEnabled());
		button->setIconSize(QSize(Common::Helpers::pointsToPixels(20), Common::Helpers::pointsToPixels(20)));
	};

	VERIFY(QObject::connect(button, &QToolButton::clicked, action, &QAction::trigger));
	VERIFY(QObject::connect(action, &QAction::changed, updateControl));

	updateControl();
	return button;
}

HeaderControlsContainer::HeaderControlsContainer()
	: m_activePage(static_cast<PageFactory::Page>(-1))
{
}

void HeaderControlsContainer::addAction(QAction* action, PageFactory::Page page, int position)
{
	// TODO: use position
	Q_UNUSED(position);
	ASSERT(action != nullptr);

	QWidget* widget = m_controlsByAction.value(action, nullptr);

	if (widget == nullptr)
	{
		widget = createControl(action);
	}

	addWidget(widget, page, position);
}

void HeaderControlsContainer::addWidget(QWidget* widget, PageFactory::Page page, int position)
{
	m_controls[page].removeOne(widget);

	if (position == -1)
	{
		m_controls[page].push_back(widget);
	}
	else
	{
		m_controls[page].insert(position, widget);
	}

	if (!m_controls[m_activePage].contains(widget))
	{
		widget->hide();
	}

	if (m_activePage == page)
	{
		emit currentControlsChanged(static_cast<int>(page));
	}
}

QWidget* HeaderControlsContainer::createControl(QAction* action) const
{
	return HeaderToolButtonCreator::createControl(action);
}

QList<QWidget*> HeaderControlsContainer::controls(PageFactory::Page page) const
{
	return m_controls[page];
}

void HeaderControlsContainer::setActivePage(PageFactory::Page page)
{
	m_activePage = page;
	emit currentControlsChanged(static_cast<int>(page));
}

}
