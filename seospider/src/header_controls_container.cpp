#include "header_controls_container.h"

namespace SeoSpider
{

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

	if (m_activePage == page)
	{
		emit currentControlsChanged(static_cast<int>(page));
	}
}

QWidget* HeaderControlsContainer::createControl(QAction* action) const
{
	QPushButton* button = new QPushButton();

	auto updateControl = [action, button]()
	{
		button->setText(action->text());
		button->setIcon(action->icon());
		button->setEnabled(action->isEnabled());
	};

	VERIFY(connect(button, &QPushButton::clicked, action, &QAction::trigger));
	VERIFY(connect(action, &QAction::changed, updateControl));

	updateControl();
	return button;
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
